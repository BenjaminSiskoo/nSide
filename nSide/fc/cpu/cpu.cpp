#include <fc/fc.hpp>

namespace Famicom {

CPU cpu;
#include "memory.cpp"
#include "io.cpp"
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

  if(system.vs()) vssystem.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizeCoprocessors();

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
  if(status.interruptPending) return interrupt();
  instruction();
}

auto CPU::load(Markup::Node node) -> bool {
  return true;
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

  function<auto (uint16, uint8) -> uint8> reader;
  function<auto (uint16, uint8) -> void> writer;

  reader = [](uint16 addr, uint8) -> uint8 { return cpu.ram[addr]; };
  writer = [](uint16 addr, uint8 data) -> void { cpu.ram[addr] = data; };
  bus.map(reader, writer, "0000-1fff", 0x800);

  reader = {&CPU::readCPU, this};
  writer = {&CPU::writeCPU, this};
  bus.map(reader, writer, "4000-4017");

  //CPU
  r.pc  = bus.read(0xfffc, r.mdr) << 0;
  r.pc |= bus.read(0xfffd, r.mdr) << 8;

  status.interruptPending = false;
  status.nmiPending = false;
  status.nmiLine = 0;
  status.irqLine = 0;
  status.apuLine = 0;

  status.rdyLine = 1;
  status.rdyAddrValid = false;
  status.rdyAddrValue = 0x0000;

  status.oamdmaPending = false;
  status.oamdmaPage = 0x00;
}

}
