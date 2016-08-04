//MOS 6507

struct CPU : Processor::R6502, Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;
  auto reset() -> void;

  //memory.cpp
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;

  //timing.cpp
  auto step(uint clocks) -> void;
  auto lastCycle() -> void;

  auto rdyLine(bool) -> void;
  auto rdyAddr(bool valid, uint16 value = 0) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  auto nmi(uint16&) -> void {}

privileged:
  struct IO {
    bool rdyLine;
    bool rdyAddrValid;
    uint16 rdyAddrValue;
  } io;
};

extern CPU cpu;
