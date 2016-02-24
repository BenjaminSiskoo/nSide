struct SMP : Thread {
  enum : bool { Threaded = false };

  SMP();

  alwaysinline auto synchronizeCPU() -> void;
  alwaysinline auto synchronizeDSP() -> void;

  auto port_read(uint_t port) -> uint_t;
  auto port_write(uint_t port, unsigned data) -> void;

  auto mmio_read(uint_t addr) -> uint_t;
  auto mmio_write(uint_t addr, uint_t data) -> void;

  auto main() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  auto disassemble_opcode(char* output, uint16_t addr) -> void;

  uint8_t iplrom[64];
  uint8_t* apuram;

//private:
  auto tick() -> void;
  alwaysinline auto op_io() -> void;
  alwaysinline auto op_read(uint16_t addr) -> uint8_t;
  alwaysinline auto op_write(uint16_t addr, uint8_t data) -> void;
  alwaysinline auto op_step() -> void;

  auto op_adc (uint8_t  x, uint8_t  y) -> uint8_t;
  auto op_addw(uint16_t x, uint16_t y) -> uint16_t;
  auto op_and (uint8_t  x, uint8_t  y) -> uint8_t;
  auto op_cmp (uint8_t  x, uint8_t  y) -> uint8_t;
  auto op_cmpw(uint16_t x, uint16_t y) -> uint16_t;
  auto op_eor (uint8_t  x, uint8_t  y) -> uint8_t;
  auto op_inc (uint8_t  x) -> uint8_t;
  auto op_dec (uint8_t  x) -> uint8_t;
  auto op_or  (uint8_t  x, uint8_t  y) -> uint8_t;
  auto op_sbc (uint8_t  x, uint8_t  y) -> uint8_t;
  auto op_subw(uint16_t x, uint16_t y) -> uint16_t;
  auto op_asl (uint8_t  x) -> uint8_t;
  auto op_lsr (uint8_t  x) -> uint8_t;
  auto op_rol (uint8_t  x) -> uint8_t;
  auto op_ror (uint8_t  x) -> uint8_t;

  struct Flags {
    alwaysinline operator uint_t() const {
      return (n << 7) | (v << 6) | (p << 5) | (b << 4)
           | (h << 3) | (i << 2) | (z << 1) | (c << 0);
    };

    alwaysinline auto operator=(uint_t data) -> uint_t {
      n = data & 0x80; v = data & 0x40; p = data & 0x20; b = data & 0x10;
      h = data & 0x08; i = data & 0x04; z = data & 0x02; c = data & 0x01;
      return data;
    }

    alwaysinline auto operator|=(uint_t data) -> uint_t { return operator=(operator uint_t() | data); }
    alwaysinline auto operator^=(uint_t data) -> uint_t { return operator=(operator uint_t() ^ data); }
    alwaysinline auto operator&=(uint_t data) -> uint_t { return operator=(operator uint_t() & data); }

    bool n, v, p, b, h, i, z, c;
  };

  struct Regs {
    uint16_t pc;
    uint8_t sp;
    union {
      uint16_t ya;
      struct { uint8_t order_lsb2(a, y); };
    };
    uint8_t x;
    Flags p;
  } regs;

  uint16_t rd, wr, dp, sp, ya, bit;

  struct Status {
    //$00f1
    bool iplrom_enable;

    //$00f2
    uint_t dsp_addr;

    //$00f8,$00f9
    uint_t ram00f8;
    uint_t ram00f9;
  } status;

  uint_t opcode_number;
  uint_t opcode_cycle;

  template<uint_t frequency>
  struct Timer {
    auto tick() -> void;
    auto tick(uint_t clocks) -> void;

    bool enable;
    uint8_t target;
    uint8_t stage1_ticks;
    uint8_t stage2_ticks;
    uint8_t stage3_ticks;
  };

  Timer<128> timer0;
  Timer<128> timer1;
  Timer< 16> timer2;

  static const uint_t cycle_count_table[256];
  uint64_t cycle_table_cpu[256];
  uint_t cycle_table_dsp[256];
  uint64_t cycle_step_cpu;
};

extern SMP smp;
