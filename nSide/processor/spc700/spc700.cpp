#include <processor/processor.hpp>
#include "spc700.hpp"

namespace Processor {

#define PC r.pc.w
#define YA r.ya.w
#define A r.ya.byte.l
#define X r.x
#define Y r.ya.byte.h
#define S r.s
#define P r.p

#define CF r.p.c
#define ZF r.p.z
#define IF r.p.i
#define HF r.p.h
#define BF r.p.b
#define PF r.p.p
#define VF r.p.v
#define NF r.p.n

#define alu (this->*op)

#include "algorithms.cpp"
#include "instructions.cpp"
#include "instruction.cpp"
#include "serialization.cpp"
#include "disassembler.cpp"

auto SPC700::idle(uint16 address) -> void {
  read(address);
}

auto SPC700::page(uint8 address) const -> uint16 {
  return PF << 8 | address;
}

auto SPC700::stack(uint8 address) const -> uint16 {
  return 1 << 8 | address;
}

auto SPC700::power() -> void {
  PC = 0x0000;
  YA = 0x0000;
  X = 0x00;
  S = 0xef;
  P = 0x02;

  r.wait = false;
  r.stop = false;
}

#undef PC
#undef YA
#undef A
#undef X
#undef Y
#undef S
#undef P

#undef CF
#undef ZF
#undef IF
#undef HF
#undef BF
#undef PF
#undef VF
#undef NF

#undef alu

}
