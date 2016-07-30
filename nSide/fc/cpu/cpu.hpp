struct CPU : Processor::R6502, Thread {
  CPU();

  static auto Enter() -> void;
  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;
  auto reset() -> void;

  //memory.cpp
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;
  auto readDisassembler(uint16 addr) -> uint8 override;

  //mmio.cpp
  auto readCPU(uint16 addr, uint8 data) -> uint8;
  auto writeCPU(uint16 addr, uint8 data) -> void;

  //timing.cpp
  auto step(uint clocks) -> void;
  auto lastCycle() -> void;
  auto nmi(uint16 &vector) -> void;

  auto oamdma() -> void;

  auto nmiLine(bool) -> void;
  auto irqLine(bool) -> void;
  auto apuLine(bool) -> void;

  auto rdyLine(bool) -> void;
  auto rdyAddr(bool valid, uint16 value = 0) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint8 ram[2 * 1024];
  vector<Thread*> coprocessors;
  vector<Thread*> peripherals;

  bool side; // VS. System; 0: main, 1: sub

//privileged:
  struct Status {
    bool interruptPending;
    bool nmiPending;
    bool nmiLine;
    bool irqLine;
    bool apuLine;

    bool rdyLine;
    bool rdyAddrValid;
    uint16 rdyAddrValue;
  } status;

  struct IO {
    bool oamdmaPending;
    uint8 oamdmaPage;
  } io;
};

extern CPU cpu;
