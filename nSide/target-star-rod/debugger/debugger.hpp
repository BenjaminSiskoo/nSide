struct Usage {
  enum : uint { Read = 4, Write = 2, Exec = 1 };
  uint8_t* data;
  uint size;

  auto allocate(uint size) -> void;
  auto reset() -> void;
  Usage();
  ~Usage();
};

struct Debugger {
  bool paused;

  struct Flags {
    bool step;
    struct CPU {
      bool stepInto;
      bool nmi;
      bool irq;
    } cpu;
    struct SMP {
      bool stepInto;
    } smp;
  } flags;

  struct Debug {
    bool cpu;
    bool smp;
  } debug;

  Usage cpuUsage;
  Usage apuUsage;
  auto loadUsage() -> void;
  auto saveUsage() -> void;
  auto resetUsage() -> void;

  auto run() -> void;
  auto echo(const string& text) -> void;
  auto resume() -> void;   //start running until breakpoint is reached
  auto suspend() -> void;  //stop running as soon as possible

  //S-CPU
  auto cpu_op_exec(uint24 addr) -> void;
  auto cpu_op_read(uint24 addr, uint8 data) -> void;
  auto cpu_op_write(uint24 addr, uint8 data) -> void;
  auto cpu_op_nmi() -> void;
  auto cpu_op_irq() -> void;

  //S-SMP
  auto smp_op_exec(uint16 addr) -> void;
  auto smp_op_read(uint16 addr, uint8 data) -> void;
  auto smp_op_write(uint16 addr, uint8 data) -> void;

  //S-PPU
  auto ppu_vramRead(uint16 addr, uint8 data) -> void;
  auto ppu_oamRead(uint16 addr, uint8 data) -> void;
  auto ppu_cgramRead(uint16 addr, uint8 data) -> void;
  auto ppu_vramWrite(uint16 addr, uint8 data) -> void;
  auto ppu_oamWrite(uint16 addr, uint8 data) -> void;
  auto ppu_cgramWrite(uint16 addr, uint8 data) -> void;

  Debugger();

  template<typename... Args> void print(Args&&... args) {
    string text(std::forward<Args>(args)...);
    echo(text);
  }
};

extern unique_pointer<Debugger> debugger;
