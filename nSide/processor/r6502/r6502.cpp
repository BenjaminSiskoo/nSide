#include <processor/processor.hpp>
#include "r6502.hpp"

namespace Processor {

#include "algorithms.cpp"
#include "disassembler.cpp"
#include "serialization.cpp"

#define L lastCycle();
#define call(op) (this->*op)()

#include "opcode_read.cpp"
#include "opcode_write.cpp"
#include "opcode_rmw.cpp"
#include "opcode_pc.cpp"
#include "opcode_misc.cpp"
#include "opcode_illegal.cpp"
#include "switch.cpp"

auto R6502::mdr() const -> uint8 {
  return r.mdr;
}

auto R6502::power() -> void {
  r.a = 0x00;
  r.x = 0x00;
  r.y = 0x00;
  r.s = 0x00;
  r.p = 0x04;
}

auto R6502::reset() -> void {
  r.mdr = 0x00;
  r.s -= 3;
  r.p.i = 1;
}

auto R6502::interrupt() -> void {
  readPC();
  readPC();
  writeSP(r.pc >> 8);
  writeSP(r.pc >> 0);
  uint16 vector = 0xfffe;  //IRQ
  nmi(vector);
  writeSP(r.p | 0x20);
  abs.l = read(vector++);
  r.p.i = 1;
L abs.h = read(vector++);
  r.pc = abs.w;
}

#undef L
#undef call

}
