struct PC10ArcadeBoard {
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

  bool vram_access;
  bool controls;
  bool ppu_output;
  bool apu_output;
  bool cpu_reset;
  bool cpu_stop;
  bool display;
  bool z80_nmi;
  bool watchdog;
  bool ppu_reset;
  uint4 channel;
  bool sram_bank;

  bool prom_test;
  bool prom_clock;
  uint7 prom_address;

  uint32 video_output[256 * 240];

  auto init() -> void;
  auto load() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto set_dip(uint16 dip) -> void;
  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;

  auto io_read(uint16 addr) -> uint8;
  auto io_write(uint16 addr, uint8 data) -> void;

  auto serialize(serializer& s) -> void;

private:
  auto video_power() -> void;
  auto update_video() -> void;
};

extern PC10ArcadeBoard pc10arcadeboard;
