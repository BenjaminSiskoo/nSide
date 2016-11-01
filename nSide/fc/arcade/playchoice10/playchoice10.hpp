struct PlayChoice10 {
  struct ScreenConfig { enum : uint {
    Dual = 2,
    Single = 1,
  };};

  auto init() -> void;
  auto load(Markup::Node node) -> bool;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto setDip(uint16 dip) -> void;

  auto serialize(serializer& s) -> void;

  uint16 dip;
  uint screenConfig;

  bool vramAccess;
  bool controls;
  bool ppuOutput;
  bool apuOutput;
  bool cpuReset;
  bool cpuStop;
  bool display;
  bool z80NMI;
  bool watchdog;
  bool ppuReset;

  struct Bus : Processor::Z80::Bus {
    auto read(uint16 addr) -> uint8 override;
    auto write(uint16 addr, uint8 data) -> void override;

    auto in(uint8 addr) -> uint8 override;
    auto out(uint8 addr, uint8 data) -> void override;

    auto power() -> void;
    auto reset() -> void;

    auto serialize(serializer& s) -> void;

    uint8 bios[0x4000];

  private:
    //memory map
    //8000-87ff: WRAM
    //8800-8bff: SRAM bank 0
    //8c00-8fff: SRAM switchable bank (0 or 1)
    //9000-97ff: VRAM
    //9800-bfff: open bus
    //c000-dfff: cartridge instruction ROM
    //e000-ffff: cartridge IO registers
    uint8 wram[0x800];
    uint8 sram[0x800];

    uint4 channel;
    bool sramBank;
    bool promTest;
    bool promClock;
    uint7 promAddress;
  } pc10bus;

  struct CPU : Processor::Z80, Thread {
    static auto Enter() -> void;
    auto main() -> void;
    auto step(uint clocks) -> void;

    auto power() -> void;
    auto reset() -> void;

    auto serialize(serializer& s) -> void;
  } pc10cpu;

  struct VideoCircuit : Thread {
    static auto Enter() -> void;
    auto main() -> void;
    auto step(uint clocks) -> void;
    auto refresh() -> void;

    auto power() -> void;
    auto reset() -> void;

    auto run(uint x, uint y) -> void;

    uint8 chrrom[0x2000 * 3];
    uint8 cgrom[0x100 * 3];

    uint8 vram[2048];

  private:
    uint32 buffer[256 * 240];
  } videoCircuit;
};

extern PlayChoice10 playchoice10;
