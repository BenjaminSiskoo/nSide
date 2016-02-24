class Background {
  struct ID { enum : uint_t { BG1, BG2, BG3, BG4 }; };
  struct Mode { enum : uint_t { BPP2, BPP4, BPP8, Mode7, Inactive }; };
  struct ScreenSize { enum : uint_t { Size32x32, Size32x64, Size64x32, Size64x64 }; };
  struct TileSize { enum : uint_t { Size8x8, Size16x16 }; };

  Background(PPU& self, uint_t id);
  ~Background();

  alwaysinline auto get_tile(uint_t hoffset, uint_t voffset) -> uint_t;
  auto offset_per_tile(uint_t x, uint_t y, uint_t& hoffset, uint_t& voffset) -> void;
  auto scanline() -> void;
  auto render() -> void;
  auto render_mode7() -> void;

  auto serialize(serializer&) -> void;

  PPU& self;

  bool priority0_enable;
  bool priority1_enable;

  struct Regs {
    uint_t mode;
    uint_t priority0;
    uint_t priority1;

    bool tile_size;
    uint_t mosaic;

    uint_t screen_addr;
    uint_t screen_size;
    uint_t tiledata_addr;

    uint_t hoffset;
    uint_t voffset;

    bool main_enable;
    bool sub_enable;
  } regs;

  uint16_t** mosaic_table;

  const uint_t id;
  uint_t opt_valid_bit;

  bool hires;
  signed width;

  uint_t tile_width;
  uint_t tile_height;

  uint_t mask_x;
  uint_t mask_y;

  uint_t scx;
  uint_t scy;

  uint_t hscroll;
  uint_t vscroll;

  uint_t mosaic_vcounter;
  uint_t mosaic_voffset;

  LayerWindow window;

  friend class PPU;
};
