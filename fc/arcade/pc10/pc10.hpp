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

  void init();
  void load();
  void unload();
  void power();
  void reset();

  void set_dip(uint16 dip);
  uint8 read(uint16 addr);
  void write(uint16 addr, uint8 data);

  uint8 io_read(uint16 addr);
  void io_write(uint16 addr, uint8 data);

  void serialize(serializer& s);

private:
  void video_power();
  void update_video();
};

extern PC10ArcadeBoard pc10arcadeboard;
