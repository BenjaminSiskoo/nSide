#include <processor/processor.hpp>
#include "z80.hpp"

namespace Processor {

#include "instruction.cpp"
#include "instructions.cpp"
#include "disassembler.cpp"

#include "legacy-lr35902-switch.cpp"
#include "serialization.cpp"

auto Z80::power() -> void {
}

auto Z80::reset() -> void {
  r.af = 0x0000;
  r.bc = 0x0000;
  r.de = 0x0000;
  r.hl = 0x0000;
  r.ix = 0x0000;
  r.iy = 0x0000;
  r.sp = 0x0000;
  r.pc = 0x0000;
  r.i  = 0x00;
  r.r  = 0x00;

  r.di = false;
  r.ei = false;
  r.im = 0;

  r.halt = false;
  r.stop = false;
  r.ime = false;
}

auto Z80::parity(uint8_t value) const -> bool {
  value ^= value >> 4;
  value ^= value >> 2;
  value ^= value >> 1;
  return !(value & 1);
}

auto Z80::interrupt(uint16 vector) -> void {
  wait();
  wait();
  wait();
  r.ime = 0;
  write(--r.sp, r.pc >> 8);
  write(--r.sp, r.pc >> 0);
  r.pc = vector;
}

}
