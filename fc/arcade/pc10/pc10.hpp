struct PC10ArcadeBoard {
  //Accessible by Z80 processor
  uint8 bios[16384];
  uint8 wram[2048];
  uint8 sram[2048];
  uint8 vram[2048];

  void init();
  void load();
  void unload();
  void power();
  void reset();

  void serialize(serializer& s);
};

extern PC10ArcadeBoard pc10arcadeboard;
