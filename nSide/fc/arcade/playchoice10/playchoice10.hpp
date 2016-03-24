struct PlayChoice10 {
  uint8 bios[16384];
  uint8 wram[2048];
  uint8 sram[2048];
  uint8 vram[2048];
  //Z80 memory map
  //0000-7fff: BIOS
  //8000-87ff: WRAM
  //8800-8bff: SRAM bank 0
  //8c00-8fff: SRAM switchable bank (0 or 1)
  //9000-97ff: VRAM
  //9800-bfff: open bus
  //c000-dfff: cartridge instruction ROM
  //e000-ffff: cartridge IO registers

  uint8 chrrom[0x2000 * 3];
  uint8 cgrom[0x100 * 3];

  uint16 dip;

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
  uint4 channel;
  bool sramBank;

  bool promTest;
  bool promClock;
  uint7 promAddress;

  uint32 videoOutput[256 * 240];

  auto init() -> void;
  auto load() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto setDip(uint16 dip) -> void;
  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;

  auto io_read(uint16 addr) -> uint8;
  auto io_write(uint16 addr, uint8 data) -> void;

  auto serialize(serializer& s) -> void;

private:
  auto videoPower() -> void;
  auto updateVideo() -> void;
};

extern PlayChoice10 playchoice10;
