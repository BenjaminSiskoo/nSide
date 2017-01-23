#include <fc/fc.hpp>

namespace Famicom {

CPU cpu;
#include "memory.cpp"
#include "io.cpp"
#include "timing.cpp"
#include "serialization.cpp"

CPU::CPU() : Processor::MOS6502(false) {
}

auto CPU::Enter() -> void {
  while(true) scheduler.synchronize(), cpu.main();
}

auto CPU::main() -> void {
  if(io.interruptPending) return interrupt();
  instruction();
}

auto CPU::load(Markup::Node node) -> bool {
  return true;
}

auto CPU::power() -> void {
  MOS6502::power();
  coprocessors.reset();

  function<auto (uint16, uint8) -> uint8> reader;
  function<auto (uint16, uint8) -> void> writer;

  reader = [](uint16 addr, uint8) -> uint8 { return cpu.ram[addr]; };
  writer = [](uint16 addr, uint8 data) -> void { cpu.ram[addr] = data; };
  bus.map(reader, writer, "0000-1fff", 0x800);

  reader = {&CPU::readCPU, this};
  writer = {&CPU::writeCPU, this};
  bus.map(reader, writer, "4000-4017");

  for(auto addr : range(0x0800)) ram[addr] = 0xff;
  ram[0x0008] = 0xf7;
  ram[0x0009] = 0xef;
  ram[0x000a] = 0xdf;
  ram[0x000f] = 0xbf;

  cpu.reset();
}

auto CPU::reset() -> void {
  create(Enter, system.colorburst() * 6.0);
  MOS6502::reset();

  //CPU
  r.pc  = bus.read(0xfffc, r.mdr) << 0;
  r.pc |= bus.read(0xfffd, r.mdr) << 8;

  io.interruptPending = false;
  io.nmiPending = false;
  io.nmiLine = 0;
  io.irqLine = 0;
  io.apuLine = 0;

  io.rdyLine = 1;
  io.rdyAddrValid = false;
  io.rdyAddrValue = 0x0000;

  io.oamdmaPending = false;
  io.oamdmaPage = 0x00;
}

}
