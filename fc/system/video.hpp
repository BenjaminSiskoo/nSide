struct Video {
  uint32_t* palette;
  void generate_palette(Emulator::Interface::PaletteMode mode);
  Video();
  ~Video();

private:
  void update();
  void update_vs();
  void update_pc10();
  void scanline();
  void init();

  uint32_t generate_color(unsigned, double, double, double, double, double);
  static const uint8_t gamma_ramp[8];
  static const uint8_t cursor[15 * 15];
  void draw_cursor(uint16_t color, int x, int y);

  friend class System;
};

extern Video video;
