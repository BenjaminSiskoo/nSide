#include <sfc/sfc.hpp>

namespace SuperFamicom {

CPU cpu;
#include "dma.cpp"
#include "memory.cpp"
#include "mmio.cpp"
#include "timing.cpp"
#include "irq.cpp"
#include "joypad.cpp"
#include "serialization.cpp"

auto CPU::interruptPending() const -> bool { return status.interruptPending; }
auto CPU::pio() const -> uint8 { return status.pio; }
auto CPU::joylatch() const -> bool { return status.joypadStrobeLatch; }

CPU::CPU() {
  PPUcounter::scanline = {&CPU::scanline, this};
}

auto CPU::step(uint clocks) -> void {
  smp.clock -= clocks * (uint64)smp.frequency;
  ppu.clock -= clocks;
  for(auto coprocessor : coprocessors) {
    coprocessor->clock -= clocks * (uint64)coprocessor->frequency;
  }
  for(auto peripheral : peripherals) {
    peripheral->clock -= clocks * (uint64)peripheral->frequency;
  }
  synchronizePeripherals();
}

auto CPU::synchronizeSMP() -> void {
  if(smp.clock < 0) co_switch(smp.thread);
}

auto CPU::synchronizePPU() -> void {
  if(ppu.clock < 0) co_switch(ppu.thread);
}

auto CPU::synchronizeCoprocessors() -> void {
  for(auto coprocessor : coprocessors) {
    if(coprocessor->clock < 0) co_switch(coprocessor->thread);
  }
}

auto CPU::synchronizePeripherals() -> void {
  for(auto peripheral : peripherals) {
    if(peripheral->clock < 0) co_switch(peripheral->thread);
  }
}

auto CPU::Enter() -> void {
  while(true) scheduler.synchronize(), cpu.main();
}

auto CPU::main() -> void {
  if(status.interruptPending) {
    status.interruptPending = false;
    if(status.nmiPending) {
      status.nmiPending = false;
      r.vector = r.e ? 0xfffa : 0xffea;
      interrupt();
      debugger.nmi();
    } else if(status.irqPending) {
      status.irqPending = false;
      r.vector = r.e ? 0xfffe : 0xffee;
      interrupt();
      debugger.irq();
    } else if(status.resetPending) {
      status.resetPending = false;
      addClocks(132);
      r.vector = 0xfffc;
      interrupt();
    } else if(status.powerPending) {
      status.powerPending = false;
      addClocks(186);
      r.pc.l = bus.read(0xfffc, r.mdr);
      r.pc.h = bus.read(0xfffd, r.mdr);
    }
  }

  debugger.execute(r.pc.d);
  instruction();
}

auto CPU::load(Markup::Node node) -> bool {
  version = max(1, min(2, node["cpu/version"].natural()));
  return true;
}

auto CPU::power() -> void {
  for(auto& byte : wram) byte = random(0x55);

  //CPU
  r.a = 0x0000;
  r.x = 0x0000;
  r.y = 0x0000;
  r.s = 0x01ff;

  //DMA
  for(auto& channel : this->channel) {
    channel.direction = 1;
    channel.indirect = true;
    channel.unused = true;
    channel.reverseTransfer = true;
    channel.fixedTransfer = true;
    channel.transferMode = 7;

    channel.targetAddress = 0xff;

    channel.sourceAddress = 0xffff;
    channel.sourceBank = 0xff;

    channel.transferSize = 0xffff;
    channel.indirectBank = 0xff;

    channel.hdmaAddress = 0xffff;
    channel.lineCounter = 0xff;
    channel.unknown = 0xff;
  }

  status.powerPending = true;
  status.interruptPending = true;
}

auto CPU::reset() -> void {
  create(Enter, system.cpuFrequency());
  coprocessors.reset();
  PPUcounter::reset();

  function<auto (uint24, uint8) -> uint8> reader;
  function<auto (uint24, uint8) -> void> writer;

  reader = {&CPU::readAPU, this};
  writer = {&CPU::writeAPU, this};
  bus.map(reader, writer, "00-3f,80-bf:2140-217f");

  reader = {&CPU::readCPU, this};
  writer = {&CPU::writeCPU, this};
  bus.map(reader, writer, "00-3f,80-bf:2180-2183,4016-4017,4200-421f");

  reader = {&CPU::readDMA, this};
  writer = {&CPU::writeDMA, this};
  bus.map(reader, writer, "00-3f,80-bf:4300-437f");

  reader = [](uint24 addr, uint8) -> uint8 { return cpu.wram[addr]; };
  writer = [](uint24 addr, uint8 data) -> void { cpu.wram[addr] = data; };
  bus.map(reader, writer, "00-3f,80-bf:0000-1fff", 0x2000);
  bus.map(reader, writer, "7e-7f:0000-ffff", 0x20000);

  //CPU
  r.pc     = 0x000000;
  r.x.h    = 0x00;
  r.y.h    = 0x00;
  r.s.h    = 0x01;
  r.d      = 0x0000;
  r.db     = 0x00;
  r.p      = 0x34;
  r.e      = 1;
  r.mdr    = 0x00;
  r.wai    = false;
  r.vector = 0xfffc;  //reset vector address

  //$2140-217f
  for(auto& port : status.port) port = 0x00;

  //$2181-$2183
  status.wramAddress = 0x000000;

  //$4016-$4017
  status.joypadStrobeLatch = 0;
  status.joypad1_bits = ~0;
  status.joypad2_bits = ~0;

  //$4200
  status.nmiEnabled = false;
  status.hirqEnabled = false;
  status.virqEnabled = false;
  status.autoJoypadPoll = false;

  //$4201
  status.pio = 0xff;

  //$4202-$4203
  status.wrmpya = 0xff;
  status.wrmpyb = 0xff;

  //$4204-$4206
  status.wrdiva = 0xffff;
  status.wrdivb = 0xff;

  //$4207-$420a
  status.hirqPos = 0x01ff;
  status.virqPos = 0x01ff;

  //$420d
  status.romSpeed = 8;

  //$4214-$4217
  status.rddiv = 0x0000;
  status.rdmpy = 0x0000;

  //$4218-$421f
  status.joy1 = 0x0000;
  status.joy2 = 0x0000;
  status.joy3 = 0x0000;
  status.joy4 = 0x0000;

  //ALU
  alu.mpyctr = 0;
  alu.divctr = 0;
  alu.shift = 0;

  //DMA
  for(auto& channel : this->channel) {
    channel.dmaEnabled = false;
    channel.hdmaEnabled = false;

    channel.hdmaCompleted = false;
    channel.hdmaDoTransfer = false;
  }

  pipe.valid = false;
  pipe.addr = 0;
  pipe.data = 0;

  //Timing
  status.clockCount = 0;
  status.lineClocks = lineclocks();

  status.irqLock = false;
  status.dramRefreshPosition = (version == 1 ? 530 : 538);
  status.dramRefreshed = false;

  status.hdmaInitPosition = (version == 1 ? 12 + 8 - dmaCounter() : 12 + dmaCounter());
  status.hdmaInitTriggered = false;

  status.hdmaPosition = 1104;
  status.hdmaTriggered = false;

  status.nmiValid      = false;
  status.nmiLine       = false;
  status.nmiTransition = false;
  status.nmiPending    = false;
  status.nmiHold       = false;

  status.irqValid      = false;
  status.irqLine       = false;
  status.irqTransition = false;
  status.irqPending    = false;
  status.irqHold       = false;

  status.resetPending = !status.powerPending;
  status.interruptPending = true;

  status.dmaActive   = false;
  status.dmaCounter  = 0;
  status.dmaClocks   = 0;
  status.dmaPending  = false;
  status.hdmaPending = false;
  status.hdmaMode    = 0;

  status.autoJoypadActive  = false;
  status.autoJoypadLatch   = false;
  status.autoJoypadCounter = 0;
  status.autoJoypadClock   = 0;
}

}
