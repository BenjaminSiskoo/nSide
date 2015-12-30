struct Video {
  Video();
  ~Video();
  auto generate_palette(Emulator::Interface::PaletteMode mode) -> void;

  uint32_t* palette = nullptr;

private:
  auto update() -> void;
  auto update_vs() -> void;
  auto update_pc10() -> void;
  auto scanline() -> void;
  auto init() -> void;

  auto generate_color(uint, double, double, double, double, double) -> uint32;
  static const uint8 gamma_ramp[8];
  static const uint8 cursor[15 * 15];
  auto draw_cursor(uint16 color, int x, int y) -> void;

  friend class System;
};

extern Video video;
