//MOS Technologies MOS6502

#pragma once

namespace Processor {

struct MOS6502 {
  virtual auto read(uint16 addr) -> uint8 = 0;
  virtual auto write(uint16 addr, uint8 data) -> void = 0;
  virtual auto lastCycle() -> void = 0;
  virtual auto nmi(uint16& vector) -> void = 0;
  virtual auto readDebugger(uint16 addr) -> uint8 { return 0; }

  //mos6502.cpp
  auto mdr() const -> uint8;
  auto power() -> void;
  auto reset() -> void;

  //memory.cpp
  auto idle() -> void;
  auto idlePageCrossed(uint16, uint16) -> void;
  auto idlePageAlways(uint16, uint16) -> void;
  auto opcode() -> uint8;
  auto operand() -> uint8;
  auto load(uint8 addr) -> uint8;
  auto store(uint8 addr, uint8 data) -> void;
  auto push(uint8 data) -> void;
  auto pull() -> uint8;

  //algorithms.cpp
  using fp = auto (MOS6502::*)(uint8) -> uint8;
  auto algorithmADC(uint8) -> uint8;
  auto algorithmAND(uint8) -> uint8;
  auto algorithmASL(uint8) -> uint8;
  auto algorithmBIT(uint8) -> uint8;
  auto algorithmCMP(uint8) -> uint8;
  auto algorithmCPX(uint8) -> uint8;
  auto algorithmCPY(uint8) -> uint8;
  auto algorithmDEC(uint8) -> uint8;
  auto algorithmEOR(uint8) -> uint8;
  auto algorithmINC(uint8) -> uint8;
  auto algorithmLD (uint8) -> uint8;
  auto algorithmLSR(uint8) -> uint8;
  auto algorithmORA(uint8) -> uint8;
  auto algorithmROL(uint8) -> uint8;
  auto algorithmROR(uint8) -> uint8;
  auto algorithmSBC(uint8) -> uint8;

  auto algorithmALR(uint8) -> uint8;
  auto algorithmANC(uint8) -> uint8;
  auto algorithmARR(uint8) -> uint8;
  auto algorithmAXS(uint8) -> uint8;
  auto algorithmDCP(uint8) -> uint8;
  auto algorithmISC(uint8) -> uint8;
  auto algorithmRLA(uint8) -> uint8;
  auto algorithmRRA(uint8) -> uint8;
  auto algorithmSLO(uint8) -> uint8;
  auto algorithmSRE(uint8) -> uint8;
  auto algorithmXAA(uint8) -> uint8;

  //instruction.cpp
  auto interrupt() -> void;
  auto instruction() -> void;

  //instructions.cpp

  auto instructionAbsoluteModify(fp alu) -> void;
  auto instructionAbsoluteModify(fp alu, uint8 index) -> void;
  auto instructionAbsoluteRead(fp alu, uint8& data) -> void;
  auto instructionAbsoluteRead(fp alu, uint8& data, uint8 index) -> void;
  auto instructionAbsoluteWrite(uint8& data) -> void;
  auto instructionAbsoluteWrite(uint8& data, uint8 index) -> void;
  auto instructionBranch(bool take) -> void;
  auto instructionClear(bool& flag) -> void;
  auto instructionImmediate(fp alu, uint8& data) -> void;
  auto instructionImplied(fp alu, uint8& data) -> void;
  auto instructionIndirectXRead(fp alu, uint8& data) -> void;
  auto instructionIndirectXWrite(uint8& data) -> void;
  auto instructionIndirectYRead(fp alu, uint8& data) -> void;
  auto instructionIndirectYWrite(uint8& data) -> void;
  auto instructionPull(uint8& data) -> void;
  auto instructionPush(uint8& data) -> void;
  auto instructionSet(bool& flag) -> void;
  auto instructionTransfer(uint8& source, uint8& target, bool flag) -> void;
  auto instructionZeroPageModify(fp alu) -> void;
  auto instructionZeroPageModify(fp alu, uint8 index) -> void;
  auto instructionZeroPageRead(fp alu, uint8& data) -> void;
  auto instructionZeroPageRead(fp alu, uint8& data, uint8 index) -> void;
  auto instructionZeroPageWrite(uint8& data) -> void;
  auto instructionZeroPageWrite(uint8& data, uint8 index) -> void;

  auto instructionIndirectXModify(fp alu) -> void;
  auto instructionIndirectYModify(fp alu) -> void;

  auto instructionBRK() -> void;
  auto instructionJMPAbsolute() -> void;
  auto instructionJMPIndirect() -> void;
  auto instructionJSRAbsolute() -> void;
  auto instructionNOP() -> void;
  auto instructionPHP() -> void;
  auto instructionPLP() -> void;
  auto instructionRTI() -> void;
  auto instructionRTS() -> void;

  auto instructionLASAbsolute(uint8 index) -> void;
  auto instructionLAXAbsolute() -> void;
  auto instructionLAXAbsolute(uint8 index) -> void;
  auto instructionLAXIndirectX() -> void;
  auto instructionLAXIndirectY() -> void;
  auto instructionLAXZeroPage() -> void;
  auto instructionLAXZeroPage(uint8 index) -> void;
  auto instructionLXAImmediate() -> void;
  auto instructionNOPAbsolute() -> void;
  auto instructionNOPAbsolute(uint8 index) -> void;
  auto instructionNOPImmediate() -> void;
  auto instructionNOPZeroPage() -> void;
  auto instructionNOPZeroPage(uint8 index) -> void;
  auto instructionSAXAbsolute() -> void;
  auto instructionSAXIndirectX() -> void;
  auto instructionSAXZeroPage() -> void;
  auto instructionSAXZeroPage(uint8 index) -> void;
  auto instructionSHAAbsolute(uint8 index) -> void;
  auto instructionSHAIndirectY() -> void;
  auto instructionSHXAbsolute(uint8 index) -> void;
  auto instructionSHYAbsolute(uint8 index) -> void;
  auto instructionSTP() -> void;
  auto instructionTASAbsolute(uint8 index) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassemble(uint16 pc) -> string;

  struct Flags {
    bool c;  //carry
    bool z;  //zero
    bool i;  //interrupt disable
    bool d;  //decimal mode
    bool v;  //overflow
    bool n;  //negative

    inline operator uint() const {
      return c << 0 | z << 1 | i << 2 | d << 3 | v << 6 | n << 7;
    }

    inline auto& operator=(uint8 data) {
      c = data.bit(0);
      z = data.bit(1);
      i = data.bit(2);
      d = data.bit(3);
      v = data.bit(6);
      n = data.bit(7);
      return *this;
    }
  };

  struct Registers {
    uint8  a;
    uint8  x;
    uint8  y;
    uint8  s;
    uint16 pc;
    Flags  p;
    uint8  mdr;
  } r;

  bool BCD = true;

  uint8 xaaNoise;
  uint8 lxaNoise;
};

}
