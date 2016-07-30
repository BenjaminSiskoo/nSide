#include <processor/processor.hpp>
#include "z80.hpp"

namespace Processor {

#include "disassembler.cpp"
#include "serialization.cpp"

#include "instructions.cpp"
#include "switch.cpp"

auto Z80::power() -> void {
}

auto Z80::reset() -> void {
  r[PC] = 0x0000;
  r[I] = 0x00;
  r[R] = 0x00;
  r.halt = false;
  r.stop = false;
  r.ei = false;
  r.ime = false;
}

auto Z80::interrupt(uint16 vector) -> void {
  io();
  io();
  io();
  r.ime = 0;
  write(--r[SP], r[PC] >> 8);
  write(--r[SP], r[PC] >> 0);
  r[PC] = vector;
}

}
