#include <fc/fc.hpp>

namespace Famicom {

CPU cpu;

#include "serialization.cpp"
#include "timing/timing.cpp"

CPU::CPU() {
}

auto CPU::step(uint clocks) -> void {
  apu.clock -= clocks;
  if(apu.clock < 0 && !scheduler.synchronizing()) co_switch(apu.thread);

  ppu.clock -= clocks;
  if(ppu.clock < 0 && !scheduler.synchronizing()) co_switch(ppu.thread);

  cartridge.clock -= clocks;
  if(cartridge.clock < 0 && !scheduler.synchronizing()) co_switch(cartridge.thread);

  device.controllerPort1->clock -= clocks * (uint64)device.controllerPort1->frequency;
  device.controllerPort2->clock -= clocks * (uint64)device.controllerPort2->frequency;
  device.expansionPort->clock -= clocks * (uint64)device.expansionPort->frequency;
  if(system.vs()) device.arcadePanel->clock -= clocks * (uint64)device.arcadePanel->frequency;
  synchronizeDevices();
}

auto CPU::synchronizeAPU() -> void {
  if(apu.clock < 0) co_switch(apu.thread);
}

auto CPU::synchronizePPU() -> void {
  if(ppu.clock < 0) co_switch(ppu.thread);
}

auto CPU::synchronizeCartridge() -> void {
  if(cartridge.clock < 0) co_switch(cartridge.thread);
}

auto CPU::synchronizeDevices() -> void {
  if(device.controllerPort1->clock < 0) co_switch(device.controllerPort1->thread);
  if(device.controllerPort2->clock < 0) co_switch(device.controllerPort2->thread);
  if(device.expansionPort->clock < 0) co_switch(device.expansionPort->thread);
  if(system.vs() && device.arcadePanel->clock < 0) co_switch(device.arcadePanel->thread);
}

auto CPU::Enter() -> void {
  while(true) scheduler.synchronize(), cpu.main();
}

auto CPU::main() -> void {
  if(status.interrupt_pending) return interrupt();
  exec();
}

auto CPU::power() -> void {
  R6502::power();

  for(auto addr : range(0x0800)) ram[addr] = 0xff;
  ram[0x0008] = 0xf7;
  ram[0x0009] = 0xef;
  ram[0x000a] = 0xdf;
  ram[0x000f] = 0xbf;
}

auto CPU::reset() -> void {
  R6502::reset();
  create(CPU::Enter, system.cpuFrequency());

  regs.pc  = bus.read(0xfffc) << 0;
  regs.pc |= bus.read(0xfffd) << 8;

  status.interrupt_pending = false;
  status.nmi_pending = false;
  status.nmi_line = 0;
  status.irq_line = 0;
  status.irq_apu_line = 0;

  status.rdy_line = 1;
  status.rdy_addr_valid = false;
  status.rdy_addr_value = 0x0000;

  status.oam_dma_pending = false;
  status.oam_dma_page = 0x00;
}

auto CPU::debugger_read(uint16 addr) -> uint8 {
  return bus.read(addr);
}

auto CPU::ram_read(uint16 addr) -> uint8 {
  return ram[addr & 0x07ff];
}

auto CPU::ram_write(uint16 addr, uint8 data) -> void {
  ram[addr & 0x07ff] = data;
}

auto CPU::read(uint16 addr) -> uint8 {
  if(!system.vs()) {
    if(addr == 0x4016) {
      return (mdr() & 0xe0) | device.controllerPort1->data() | device.expansionPort->data1();
    }

    if(addr == 0x4017) {
      return (mdr() & 0xe0) | device.controllerPort2->data() | device.expansionPort->data2();
    }
  } else { // if using VS. System
    if(addr >= 0x4016 && addr <= 0x5fff) return vsarcadeboard.read(addr);
  }

  return apu.read(addr);
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  if(addr == 0x4014) {
    status.oam_dma_page = data;
    status.oam_dma_pending = true;
  }

  if(!system.vs()) {
    if(addr == 0x4016) {
      device.controllerPort1->latch(data & 1);
      device.controllerPort2->latch(data & 1);
      device.expansionPort->latch(data & 1);
    }
  } else { // if using VS. System
    if(addr >= 0x4016 && addr <= 0x5fff) vsarcadeboard.write(addr, data);
  }

  return apu.write(addr, data);
}

}
