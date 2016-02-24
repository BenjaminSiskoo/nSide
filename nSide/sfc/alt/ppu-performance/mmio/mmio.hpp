public:
  auto mmio_read(uint_t addr, uint8_t data) -> uint8_t;
  auto mmio_write(uint_t addr, uint8_t data) -> void;

private:

struct Regs {
  //internal
  uint8_t ppu1_mdr;
  uint8_t ppu2_mdr;

  uint16_t vram_readbuffer;
  uint8_t oam_latchdata;
  uint8_t cgram_latchdata;
  uint8_t bgofs_latchdata;
  uint8_t mode7_latchdata;

  bool counters_latched;
  bool latch_hcounter;
  bool latch_vcounter;

  //$2100
  bool display_disable;
  uint_t display_brightness;

  //$2102-$2103
  uint16_t oam_baseaddr;
  uint16_t oam_addr;
  bool oam_priority;

  //$2105
  bool bg3_priority;
  uint_t bgmode;

  //$210d
  uint16_t mode7_hoffset;

  //$210e
  uint16_t mode7_voffset;

  //$2115
  bool vram_incmode;
  uint_t vram_mapping;
  uint_t vram_incsize;

  //$2116-$2117
  uint16_t vram_addr;

  //$211a
  uint_t mode7_repeat;
  bool mode7_vflip;
  bool mode7_hflip;

  //$211b-$2120
  uint16_t m7a;
  uint16_t m7b;
  uint16_t m7c;
  uint16_t m7d;
  uint16_t m7x;
  uint16_t m7y;

  //$2121
  uint16_t cgram_addr;

  //$2126-$212a
  uint_t window_one_left;
  uint_t window_one_right;
  uint_t window_two_left;
  uint_t window_two_right;

  //$2133
  bool mode7_extbg;
  bool pseudo_hires;
  bool overscan;
  bool interlace;

  //$213c
  uint16_t hcounter;

  //$213d
  uint16_t vcounter;
} regs;

auto get_vram_addr() -> uint16_t;
auto vram_read(uint_t addr) -> uint8_t;
auto vram_write(uint_t addr, uint8_t data) -> void;

auto oam_read(uint_t addr) -> uint8_t;
auto oam_write(uint_t addr, uint8_t data) -> void;

auto cgram_read(uint_t addr) -> uint8_t;
auto cgram_write(uint_t addr, uint8_t data) -> void;

auto mmio_update_video_mode() -> void;
auto mmio_reset() -> void;
