#include <a2600/a2600.hpp>

namespace Atari2600 {

CPU cpu;
#include "memory.cpp"
#include "timing.cpp"
#include "serialization.cpp"

auto CPU::Enter() -> void {
  while(true) scheduler.synchronize(), cpu.main();
}

auto CPU::main() -> void {
  instruction();
}

auto CPU::load(Markup::Node node) -> bool {
  return true;
}

auto CPU::power() -> void {
  R6502::power();
  create(Enter, system.colorburst() / 3.0);
}

auto CPU::reset() -> void {
  R6502::reset();

  //CPU
  r.pc  = cartridge.access(0xfffc, r.mdr) << 0;
  r.pc |= cartridge.access(0xfffd, r.mdr) << 8;

  io.rdyLine = 1;
  io.rdyAddrValid = false;
  io.rdyAddrValue = 0x0000;
}

}
