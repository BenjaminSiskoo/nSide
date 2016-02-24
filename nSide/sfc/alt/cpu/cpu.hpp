struct CPU : Processor::R65816, Thread, public PPUcounter {
  enum : bool { Threaded = true };

  CPU();

  alwaysinline auto step(uint_t clocks) -> void;
  alwaysinline auto synchronizeSMP() -> void;
  auto synchronizePPU() -> void;
  auto synchronizeCoprocessors() -> void;
  auto synchronizeDevices() -> void;

  auto pio() -> uint8_t;
  auto joylatch() -> bool;
  auto interrupt_pending() -> bool;
  auto port_read(uint8_t port) -> uint8_t;
  auto port_write(uint8_t port, uint8_t data) -> void;
  auto mmio_read(uint_t addr, uint8_t data) -> uint8_t;
  auto mmio_write(uint_t addr, uint8_t data) -> void;

  auto op_io() -> void;
  auto op_read(uint_t addr) -> uint8_t;
  auto op_write(uint_t addr, uint8_t data) -> void;

  auto main() -> void;
  auto enable() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  uint8_t wram[128 * 1024];
  vector<Thread*> coprocessors;

private:
  //cpu
  static auto Enter() -> void;

  //timing
  auto queue_event(uint_t id) -> void;
  auto last_cycle() -> void;
  auto add_clocks(uint_t clocks) -> void;
  auto scanline() -> void;
  auto run_auto_joypad_poll() -> void;

  struct QueueEvent {
    enum : uint_t {
      DramRefresh,
      HdmaRun,
    };
  };
  nall::priority_queue<uint_t> queue;

  //memory
  auto speed(uint_t addr) const -> uint_t;

  //dma
  auto dma_transfer_valid(uint8_t bbus, uint_t abus) -> bool;
  auto dma_addr_valid(uint_t abus) -> bool;
  auto dma_read(uint_t abus) -> uint8_t;
  auto dma_write(bool valid, uint_t addr, uint8_t data) -> void;
  auto dma_transfer(bool direction, uint8_t bbus, uint_t abus) -> void;
  auto dma_bbus(uint_t i, uint_t index) -> uint8_t;
  auto dma_addr(uint_t i) -> uint_t;
  auto hdma_addr(uint_t i) -> uint_t;
  auto hdma_iaddr(uint_t i) -> uint_t;
  auto dma_run() -> void;
  auto hdma_active_after(uint_t i) -> bool;
  auto hdma_update(uint_t i) -> void;
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

    uint_t wram_addr;

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

    uint_t rom_speed;

    uint16_t rddiv;
    uint16_t rdmpy;

    uint8_t joy1l, joy1h;
    uint8_t joy2l, joy2h;
    uint8_t joy3l, joy3h;
    uint8_t joy4l, joy4h;
  } status;
};

extern CPU cpu;
