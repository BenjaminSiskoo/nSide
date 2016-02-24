class Screen {
  Screen(PPU& self);

  alwaysinline auto get_palette(uint_t color) -> uint_t;
  auto get_direct_color(uint_t palette, uint_t tile) -> uint_t;
  alwaysinline auto addsub(uint_t x, uint_t y, bool halve) -> uint16_t;
  auto scanline() -> void;
  auto render_black() -> void;
  alwaysinline auto get_pixel_main(uint_t x) -> uint16_t;
  alwaysinline auto get_pixel_sub(uint_t x) -> uint16_t;
  auto render() -> void;

  auto serialize(serializer&) -> void;

  PPU& self;

  struct Regs {
    bool addsub_mode;
    bool direct_color;

    bool color_mode;
    bool color_halve;
    bool color_enable[7];

    uint_t color_b;
    uint_t color_g;
    uint_t color_r;
    uint_t color;
  } regs;

  struct Output {
    alwaysinline auto plot_main(uint_t x, uint_t color, uint_t priority, uint_t source) -> void;
    alwaysinline auto plot_sub(uint_t x, uint_t color, uint_t priority, uint_t source) -> void;

    struct Pixel {
      uint_t color;
      uint_t priority;
      uint_t source;
    } main[256], sub[256];
  } output;

  ColorWindow window;

  friend class PPU;
};
