namespace SuperFamicom {
  struct Debugger {
    struct CPU {
      debug_function<auto (uint24) -> void> execute;
      debug_function<auto (uint24, uint8) -> void> read;
      debug_function<auto (uint24, uint8) -> void> write;
      debug_function<auto () -> void> nmi;
      debug_function<auto () -> void> irq;
    } cpu;

    struct SMP {
      debug_function<auto (uint16) -> void> execute;
      debug_function<auto (uint16, uint8) -> void> read;
      debug_function<auto (uint16, uint8) -> void> write;
    } smp;
  };

  extern Debugger debugger;
}
