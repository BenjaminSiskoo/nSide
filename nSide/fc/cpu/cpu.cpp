#include <fc/fc.hpp>

namespace Famicom {

CPU cpu;

#include "serialization.cpp"
#include "timing/timing.cpp"

CPU::CPU() {
}

auto CPU::step(uint clocks) -> void {
  apu.clock -= clocks;
  if(apu.clock < 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(apu.thread);

  ppu.clock -= clocks;
  if(ppu.clock < 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(ppu.thread);

  cartridge.clock -= clocks;
  if(cartridge.clock < 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(cartridge.thread);

  device.controllerPort1->clock -= clocks * (uint64)device.controllerPort1->frequency;
  device.controllerPort2->clock -= clocks * (uint64)device.controllerPort2->frequency;
  device.expansionPort->clock -= clocks * (uint64)device.expansionPort->frequency;
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
}

auto CPU::Enter() -> void { cpu.enter(); }

auto CPU::enter() -> void {
  while(true) {
    if(scheduler.sync == Scheduler::SynchronizeMode::All) {
      scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
    }

    cpu.main();
  }
}

void CPU::main() {
  if(status.interrupt_pending) {
    interrupt();
    return;
  }

  exec();
}

void CPU::power() {
  R6502::power();

  for(uint addr = 0; addr < 0x0800; addr++) ram[addr] = 0xff;
  ram[0x0008] = 0xf7;
  ram[0x0009] = 0xef;
  ram[0x000a] = 0xdf;
  ram[0x000f] = 0xbf;
}

void CPU::reset() {
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

uint8 CPU::debugger_read(uint16 addr) {
  return bus.read(addr);
}

uint8 CPU::ram_read(uint16 addr) {
  return ram[addr & 0x07ff];
}

void CPU::ram_write(uint16 addr, uint8 data) {
  ram[addr & 0x07ff] = data;
}

uint8 CPU::read(uint16 addr) {
  if(system.revision() != System::Revision::VSSystem) {
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

void CPU::write(uint16 addr, uint8 data) {
  if(addr == 0x4014) {
    status.oam_dma_page = data;
    status.oam_dma_pending = true;
  }

  if(system.revision() != System::Revision::VSSystem) {
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
