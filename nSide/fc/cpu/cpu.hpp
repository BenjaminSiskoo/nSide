struct CPU : Processor::R6502, Thread {
  CPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeAPU() -> void;
  auto synchronizePPU() -> void;
  auto synchronizeCartridge() -> void;
  auto synchronizeCoprocessors() -> void;
  auto synchronizePeripherals() -> void;

  auto ram_read(uint16 addr) -> uint8;
  auto ram_write(uint16 addr, uint8 data) -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto power() -> void;
  auto reset() -> void;

  //memory.cpp
  auto read(uint16 addr) -> uint8 override;
  auto write(uint16 addr, uint8 data) -> void override;
  auto disassemblerRead(uint16 addr) -> uint8 override;

  //mmio.cpp
  auto cpuPortRead(uint16 addr, uint8 data) -> uint8;
  auto cpuPortWrite(uint16 addr, uint8 data) -> void;

  //timing.cpp
  auto addClocks(uint clocks) -> void;
  auto lastCycle() -> void;
  auto nmi(uint16 &vector) -> void;

  auto oam_dma() -> void;

  auto set_nmi_line(bool) -> void;
  auto set_irq_line(bool) -> void;
  auto set_irq_apu_line(bool) -> void;

  auto set_rdy_line(bool) -> void;
  auto set_rdy_addr(bool valid, uint16 value = 0) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint8 ram[2 * 1024];
  vector<Thread*> coprocessors;
  vector<Thread*> peripherals;

  bool side; // VS. System; 0: main, 1: sub

//privileged:
  struct Status {
    bool interrupt_pending;
    bool nmi_pending;
    bool nmi_line;
    bool irq_line;
    bool irq_apu_line;

    bool rdy_line;
    bool rdy_addr_valid;
    uint16 rdy_addr_value;

    bool oam_dma_pending;
    uint8 oam_dma_page;
  } status;
};

extern CPU cpu;
