struct CPU : Processor::R65816, Thread, public PPUcounter {
  enum : bool { Threaded = true };

  auto interruptPending() const -> bool override;
  auto pio() const -> uint8_t;
  auto joylatch() const -> bool;

  CPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeSMP() -> void;
  auto synchronizePPU() -> void;
  auto synchronizeCoprocessors() -> void;
  auto synchronizeDevices() -> void;

  auto portRead(uint8 port) -> uint8;
  auto portWrite(uint8 port, uint8 data) -> void;

  auto apuPortRead(uint24 addr, uint8 data) -> uint8;
  auto cpuPortRead(uint24 addr, uint8 data) -> uint8;
  auto dmaPortRead(uint24 addr, uint8 data) -> uint8;
  auto apuPortWrite(uint24 addr, uint8 data) -> void;
  auto cpuPortWrite(uint24 addr, uint8 data) -> void;
  auto dmaPortWrite(uint24 addr, uint8 data) -> void;

  auto io() -> void;
  auto read(uint24 addr) -> uint8;
  auto write(uint24 addr, uint8 data) -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto enable() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  uint8_t wram[128 * 1024];
  vector<Thread*> coprocessors;

private:
  //timing
  auto queue_event(uint id) -> void;
  auto lastCycle() -> void;
  auto addClocks(uint clocks) -> void;
  auto scanline() -> void;
  auto run_auto_joypad_poll() -> void;

  struct QueueEvent {
    enum : uint {
      DramRefresh,
      HdmaRun,
    };
  };
  nall::priority_queue<uint> queue;

  //memory
  auto speed(uint addr) const -> uint;

  //dma
  auto dma_transfer_valid(uint8_t bbus, uint abus) -> bool;
  auto dma_addr_valid(uint abus) -> bool;
  auto dma_read(uint abus) -> uint8_t;
  auto dma_write(bool valid, uint addr, uint8_t data) -> void;
  auto dma_transfer(bool direction, uint8_t bbus, uint abus) -> void;
  auto dma_bbus(uint i, uint index) -> uint8_t;
  auto dma_addr(uint i) -> uint;
  auto hdma_addr(uint i) -> uint;
  auto hdma_iaddr(uint i) -> uint;
  auto dma_run() -> void;
  auto hdma_active_after(uint i) -> bool;
  auto hdma_update(uint i) -> void;
  auto hdma_run() -> void;
  auto hdma_init() -> void;
  auto dma_reset() -> void;

  //registers
  uint8_t port_data[4];

  struct Channel {
    bool dma_enabled;
    bool hdma_enabled;

    bool direction;
    bool indirect;
    bool unused;
    bool reverse_transfer;
    bool fixed_transfer;
    uint8_t transfer_mode;

    uint8_t dest_addr;
    uint16_t source_addr;
    uint8_t source_bank;

    union {
      uint16_t transfer_size;
      uint16_t indirect_addr;
    };

    uint8_t indirect_bank;
    uint16_t hdma_addr;
    uint8_t line_counter;
    uint8_t unknown;

    bool hdma_completed;
    bool hdma_do_transfer;
  } channel[8];

  struct Status {
    bool nmi_valid;
    bool nmi_line;
    bool nmi_transition;
    bool nmi_pending;

    bool irq_valid;
    bool irq_line;
    bool irq_transition;
    bool irq_pending;

    bool irq_lock;
    bool hdma_pending;

    uint wram_addr;

    bool joypad_strobe_latch;

    bool nmi_enabled;
    bool virq_enabled;
    bool hirq_enabled;
    bool auto_joypad_poll_enabled;

    uint8_t pio;

    uint8_t wrmpya;
    uint8_t wrmpyb;
    uint16_t wrdiva;
    uint8_t wrdivb;

    uint16_t htime;
    uint16_t vtime;

    uint rom_speed;

    uint16_t rddiv;
    uint16_t rdmpy;

    uint8_t joy1l, joy1h;
    uint8_t joy2l, joy2h;
    uint8_t joy3l, joy3h;
    uint8_t joy4l, joy4h;
  } status;
};

extern CPU cpu;
