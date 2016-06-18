struct CPU : Processor::R6502, Thread {
  CPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeAPU() -> void;
  auto synchronizePPU() -> void;
  auto synchronizeCartridge() -> void;
  auto synchronizeCoprocessors() -> void;
  auto synchronizePeripherals() -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto power() -> void;
  auto reset() -> void;

  //memory.cpp
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;
  auto disassemblerRead(uint16 addr) -> uint8 override;

  //mmio.cpp
  auto readCPU(uint16 addr, uint8 data) -> uint8;
  auto writeCPU(uint16 addr, uint8 data) -> void;

  //timing.cpp
  auto addClocks(uint clocks) -> void;
  auto lastCycle() -> void;
  auto nmi(uint16 &vector) -> void;

  auto oamDMA() -> void;

  auto setNMILine(bool) -> void;
  auto setIRQLine(bool) -> void;
  auto setIRQAPULine(bool) -> void;

  auto setRDYLine(bool) -> void;
  auto setRDYAddress(bool valid, uint16 value = 0) -> void;

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
    bool irqAPULine;

    bool rdyLine;
    bool rdyAddressValid;
    uint16 rdyAddressValue;

    bool oamDMAPending;
    uint8 oamDMAPage;
  } status;
};

extern CPU cpu;
