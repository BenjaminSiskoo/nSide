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

    struct PPU {
      struct VRAM {
        debug_function<auto (uint16, uint16) -> void> read;
        debug_function<auto (bool, uint16, uint8) -> void> write;
      } vram;
      struct OAM {
        debug_function<auto (uint10, uint8) -> void> read;
        debug_function<auto (uint10, uint8) -> void> write;
      } oam;
      struct CGRAM {
        debug_function<auto (bool, uint8, uint8) -> void> read;
        debug_function<auto (uint8, uint15) -> void> write;
      } cgram;
    } ppu;
  };

  extern Debugger debugger;
}
