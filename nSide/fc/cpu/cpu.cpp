#include <fc/fc.hpp>

namespace Famicom {

CPU cpu;
#include "memory.cpp"
#include "mmio.cpp"
#include "timing.cpp"
#include "serialization.cpp"

CPU::CPU() {
}

auto CPU::step(uint clocks) -> void {
  apu.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizeAPU();

  ppu.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizePPU();

  cartridge.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizeCartridge();

  if(system.vs()) {
    vssystem.clock -= clocks;
    if(vssystem.clock < 0 && !scheduler.synchronizing()) co_switch(vssystem.thread);
  }

  for(auto peripheral : peripherals) {
    peripheral->clock -= clocks * (uint64)peripheral->frequency;
  }
  synchronizePeripherals();
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
  if(status.interrupt_pending) return interrupt();
  instruction();
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

  regs.pc  = bus.read(0xfffc, regs.mdr) << 0;
  regs.pc |= bus.read(0xfffd, regs.mdr) << 8;

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

auto CPU::ram_read(uint16 addr) -> uint8 {
  return ram[addr & 0x07ff];
}

auto CPU::ram_write(uint16 addr, uint8 data) -> void {
  ram[addr & 0x07ff] = data;
}

}
