struct CPU : Processor::R6502, Thread {
  CPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeAPU() -> void;
  auto synchronizePPU() -> void;
  auto synchronizeCartridge() -> void;
  auto synchronizeDevices() -> void;

  auto ram_read(uint16 addr) -> uint8;
  auto ram_write(uint16 addr, uint8 data) -> void;

  auto main() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  uint8 ram[0x0800];

  bool side; // VS. System; 0: main, 1: sub

//privileged:
  #include "memory/memory.hpp"
  #include "mmio/mmio.hpp"
  #include "timing/timing.hpp"

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

  static auto Enter() -> void;
};

extern CPU cpu;