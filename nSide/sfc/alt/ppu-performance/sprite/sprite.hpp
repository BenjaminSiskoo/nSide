class Sprite {
  Sprite(PPU& self);

  auto frame() -> void;
  auto update_list(uint_t addr, uint8_t data) -> void;
  auto address_reset() -> void;
  auto set_first() -> void;
  alwaysinline auto on_scanline(uint_t sprite) -> bool;
  auto render() -> void;

  auto serialize(serializer&) -> void;

  PPU& self;

  bool priority0_enable;
  bool priority1_enable;
  bool priority2_enable;
  bool priority3_enable;

  struct Regs {
    uint_t priority0;
    uint_t priority1;
    uint_t priority2;
    uint_t priority3;

    uint_t base_size;
    uint_t nameselect;
    uint_t tiledata_addr;
    uint_t first_sprite;

    bool main_enable;
    bool sub_enable;

    bool interlace;

    bool time_over;
    bool range_over;
  } regs;

  struct List {
    uint_t width;
    uint_t height;
    uint_t x;
    uint_t y;
    uint_t character;
    bool use_nameselect;
    bool vflip;
    bool hflip;
    uint_t palette;
    uint_t priority;
    bool size;
  } list[128];
  bool list_valid;

  uint8_t itemlist[32];
  struct TileList {
    uint_t x;
    uint_t y;
    uint_t priority;
    uint_t palette;
    uint_t tile;
    bool hflip;
  } tilelist[34];

  struct Output {
    uint8_t palette[256];
    uint8_t priority[256];
  } output;

  LayerWindow window;

  friend class PPU;
};
