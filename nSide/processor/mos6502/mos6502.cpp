#include <processor/processor.hpp>
#include "mos6502.hpp"

namespace Processor {

#include "algorithms.cpp"
#include "disassembler.cpp"
#include "serialization.cpp"

#define L lastCycle();
#define call(op) (this->*op)()

#include "instruction.cpp"
#include "instructions-read.cpp"
#include "instructions-write.cpp"
#include "instructions-rmw.cpp"
#include "instructions-pc.cpp"
#include "instructions-misc.cpp"
#include "instructions-illegal.cpp"

MOS6502::MOS6502(bool allowBCD) : allowBCD(allowBCD) {
}

auto MOS6502::interrupt() -> void {
  idle();
  idle();
  writeSP(r.pc.h);
  writeSP(r.pc.l);
  uint16 vector = 0xfffe;  //IRQ
  nmi(vector);
  writeSP(r.p | 0x20);
  abs.l = read(vector + 0);
  r.p.i = 1;
L abs.h = read(vector + 1);
  r.pc = abs.w;
}

auto MOS6502::mdr() const -> uint8 {
  return r.mdr;
}

auto MOS6502::power() -> void {
  r.a = 0x00;
  r.x = 0x00;
  r.y = 0x00;
  r.s = 0x00;
  r.p = 0x04;
}

auto MOS6502::reset() -> void {
  r.mdr = 0x00;
  r.s -= 3;
  r.p.i = 1;
}

#undef L
#undef call

}
