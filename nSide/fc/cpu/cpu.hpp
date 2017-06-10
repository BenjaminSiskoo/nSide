struct CPU : Processor::MOS6502, Thread {
  CPU(bool side);

  static auto Enter() -> void;
  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;
  auto reset() -> void;

  //memory.cpp
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;
  auto readDebugger(uint16 addr) -> uint8 override;

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

  const bool side;  //0: main, 1: sub (VS. System only)

//private:
  struct IO {
    bool interruptPending;
    bool nmiPending;
    bool nmiLine;
    bool irqLine;
    bool apuLine;

    bool rdyLine;
    bool rdyAddrValid;
    uint16 rdyAddrValue;

    bool oamdmaPending;
    uint8 oamdmaPage;
  } io;
};

extern CPU cpu0;
extern CPU cpu1;
