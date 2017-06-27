#pragma once

namespace Processor {

struct SPC700 {
  virtual auto read(uint16 addr) -> uint8 = 0;
  virtual auto write(uint16 addr, uint8 data) -> void = 0;
  virtual auto synchronizing() const -> bool = 0;

  virtual auto readDisassembler(uint16 addr) -> uint8 { return 0; }

  //spc700.cpp
  auto power() -> void;

  //instruction.cpp
  auto instruction() -> void;

  //memory.cpp
  auto idle() -> void;
  auto fetch() -> uint8;
  auto pull() -> uint8;
  auto push(uint8 data) -> void;
  auto load(uint8 addr) -> uint8;
  auto store(uint8 addr, uint8 data) -> void;

  //algorithms.cpp
  auto algorithmADC(uint8, uint8) -> uint8;
  auto algorithmAND(uint8, uint8) -> uint8;
  auto algorithmASL(uint8) -> uint8;
  auto algorithmCMP(uint8, uint8) -> uint8;
  auto algorithmDEC(uint8) -> uint8;
  auto algorithmEOR(uint8, uint8) -> uint8;
  auto algorithmINC(uint8) -> uint8;
  auto algorithmLD (uint8, uint8) -> uint8;
  auto algorithmLSR(uint8) -> uint8;
  auto algorithmOR (uint8, uint8) -> uint8;
  auto algorithmROL(uint8) -> uint8;
  auto algorithmROR(uint8) -> uint8;
  auto algorithmSBC(uint8, uint8) -> uint8;
  auto algorithmST (uint8, uint8) -> uint8;
  auto algorithmADW(uint16, uint16) -> uint16;
  auto algorithmCPW(uint16, uint16) -> uint16;
  auto algorithmLDW(uint16, uint16) -> uint16;
  auto algorithmSBW(uint16, uint16) -> uint16;

  //instructions.cpp
  using fps = auto (SPC700::*)(uint8) -> uint8;
  using fpb = auto (SPC700::*)(uint8, uint8) -> uint8;
  using fpw = auto (SPC700::*)(uint16, uint16) -> uint16;

  auto instructionAbsoluteBitModify(uint3) -> void;
  auto instructionAbsoluteRead(fpb, uint8&) -> void;
  auto instructionAbsoluteModify(fps) -> void;
  auto instructionAbsoluteWrite(uint8&) -> void;
  auto instructionAbsoluteIndexedRead(fpb, uint8&) -> void;
  auto instructionAbsoluteIndexedWrite(uint8&) -> void;
  auto instructionBranch(bool) -> void;
  auto instructionDirectRead(fpb, uint8&) -> void;
  auto instructionDirectModify(fps) -> void;
  auto instructionDirectWrite(uint8&) -> void;
  auto instructionDirectWriteDirect(fpb) -> void;
  auto instructionDirectWriteImmediate(fpb) -> void;
  auto instructionDirectReadWord(fpw) -> void;
  auto instructionDirectModifyWord(int) -> void;
  auto instructionDirectIndexedRead(fpb, uint8&, uint8&) -> void;
  auto instructionDirectIndexedModify(fps, uint8&) -> void;
  auto instructionDirectIndexedWrite(uint8&, uint8&) -> void;
  auto instructionFlagClear(bool&) -> void;
  auto instructionFlagSet(bool&) -> void;
  auto instructionImmediateRead(fpb, uint8&) -> void;
  auto instructionImpliedModify(fps, uint8&) -> void;
  auto instructionIndexedIndirectRead(fpb, uint8&) -> void;
  auto instructionIndexedIndirectWrite(uint8&, uint8&) -> void;
  auto instructionIndirectIndexedRead(fpb, uint8&) -> void;
  auto instructionIndirectIndexedWrite(uint8&, uint8&) -> void;
  auto instructionIndirectXRead(fpb) -> void;
  auto instructionIndirectXWrite(uint8&) -> void;
  auto instructionIndirectXIncrementRead(uint8&) -> void;
  auto instructionIndirectXIncrementWrite(uint8&) -> void;
  auto instructionIndirectXWriteIndirectY(fpb) -> void;
  auto instructionPull(uint8&) -> void;
  auto instructionPush(uint8) -> void;
  auto instructionTransfer(uint8&, uint8&) -> void;

  auto instructionBBC(uint3) -> void;
  auto instructionBBS(uint3) -> void;
  auto instructionBNEDirect() -> void;
  auto instructionBNEDirectDecrement() -> void;
  auto instructionBNEDirectX() -> void;
  auto instructionBNEYDecrement() -> void;
  auto instructionBRK() -> void;
  auto instructionCLR(uint3) -> void;
  auto instructionCLV() -> void;
  auto instructionCMC() -> void;
  auto instructionDAA() -> void;
  auto instructionDAS() -> void;
  auto instructionDIV() -> void;
  auto instructionJMPAbsolute() -> void;
  auto instructionJMPIndirectX() -> void;
  auto instructionJSPDirect() -> void;
  auto instructionJSRAbsolute() -> void;
  auto instructionJST(uint4) -> void;
  auto instructionMUL() -> void;
  auto instructionNOP() -> void;
  auto instructionPLP() -> void;
  auto instructionRTI() -> void;
  auto instructionRTS() -> void;
  auto instructionSET(uint3) -> void;
  auto instructionSTP() -> void;
  auto instructionSTWDirect() -> void;
  auto instructionTRBAbsolute() -> void;
  auto instructionTSBAbsolute() -> void;
  auto instructionWAI() -> void;
  auto instructionXCN() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassemble(uint16 addr, bool p) -> string;

  struct Flags {
    bool c;  //carry
    bool z;  //zero
    bool i;  //interrupt disable
    bool h;  //half-carry
    bool b;  //break
    bool p;  //page
    bool v;  //overflow
    bool n;  //negative

    inline operator uint() const {
      return c << 0 | z << 1 | i << 2 | h << 3 | b << 4 | p << 5 | v << 6 | n << 7;
    }

    inline auto& operator=(uint8 data) {
      c = data.bit(0);
      z = data.bit(1);
      i = data.bit(2);
      h = data.bit(3);
      b = data.bit(4);
      p = data.bit(5);
      v = data.bit(6);
      n = data.bit(7);
      return *this;
    }
  };

  struct Registers {
    union Pair {
      Pair() : w(0) {}
      uint16 w;
      struct Byte { uint8 order_lsb2(l, h); } byte;
    } pc, ya;
    uint8 x, s;
    Flags p;

    bool wai = false;
    bool stp = false;
  } r;
};

}
