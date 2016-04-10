#include "video.hpp"

struct PPU : Thread, PPUcounter {
  alwaysinline auto interlace() const -> bool { return display.interlace; }
  alwaysinline auto overscan() const -> bool { return display.overscan; }
  alwaysinline auto vdisp() const -> uint { return !regs.overscan ? 225 : 240; }

  PPU();
  ~PPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeCPU() -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto serialize(serializer&) -> void;

  //memory.cpp
  alwaysinline auto getVramAddress() -> uint16;
  alwaysinline auto vramRead(uint16 addr) -> uint8;
  alwaysinline auto vramWrite(uint16 addr, uint8 data) -> void;
  alwaysinline auto oamRead(uint16 addr) -> uint8;
  alwaysinline auto oamWrite(uint16 addr, uint8 data) -> void;
  alwaysinline auto cgramRead(uint16 addr) -> uint8;
  alwaysinline auto cgramWrite(uint16 addr, uint8 data) -> void;

  //mmio.cpp
  auto read(uint24 addr, uint8 data) -> uint8;
  auto write(uint24 addr, uint8 data) -> void;
  auto latchCounters() -> void;

  uint8 vram[128 * 1024];
  uint8 oam[544];
  uint8 cgram[512];

  //debugger functions
  auto exportRegisters(string &markup) -> void;

privileged:
  uint ppu1_version = 1;
  uint ppu2_version = 3;

  uint32* output = nullptr;

  struct {
    bool interlace;
    bool overscan;
  } display;

  alwaysinline auto addClocks(uint) -> void;

  auto scanline() -> void;
  auto render_scanline() -> void;
  auto frame() -> void;

  auto render_line() -> void;
  auto update_oam_status() -> void;

  struct Registers {
    //open bus support
    uint8 ppu1_mdr;
    uint8 ppu2_mdr;

    //bg line counters
    uint16 bg_y[4];

    //internal state
    uint16 ioamaddr;
    uint16 icgramaddr;

    //$2100
    bool   display_disabled;
    uint8  display_brightness;

    //$2101
    uint8  oam_basesize;
    uint8  oam_nameselect;
    uint16 oam_tdaddr;

    //$2102-$2103
    uint16 oam_baseaddr;
    uint16 oam_addr;
    bool   oam_priority;
    uint8  oam_firstsprite;

    //$2104
    uint8  oam_latchdata;

    //$2105
    bool   bg_tilesize[4];
    bool   bg3_priority;
    uint8  bg_mode;

    //$2106
    uint8  mosaic_size;
    bool   mosaic_enabled[4];
    uint16 mosaic_countdown;

    //$2107-$210a
    uint16 bg_scaddr[4];
    uint8  bg_scsize[4];

    //$210b-$210c
    uint16 bg_tdaddr[4];

    //$210d-$2114
    uint8  bg_ofslatch;
    uint16 m7_hofs, m7_vofs;
    uint16 bg_hofs[4];
    uint16 bg_vofs[4];

    //$2115
    bool   vram_incmode;
    uint8  vram_mapping;
    uint8  vram_incsize;

    //$2116-$2117
    uint16 vram_addr;

    //$211a
    uint8  mode7_repeat;
    bool   mode7_vflip;
    bool   mode7_hflip;

    //$211b-$2120
    uint8  m7_latch;
    uint16 m7a, m7b, m7c, m7d, m7x, m7y;

    //$2121
    uint16 cgram_addr;

    //$2122
    uint8  cgram_latchdata;

    //$2123-$2125
    bool   window1_enabled[6];
    bool   window1_invert [6];
    bool   window2_enabled[6];
    bool   window2_invert [6];

    //$2126-$2129
    uint8  window1_left, window1_right;
    uint8  window2_left, window2_right;

    //$212a-$212b
    uint8  window_mask[6];

    //$212c-$212d
    bool   bg_enabled[5], bgsub_enabled[5];

    //$212e-$212f
    bool   window_enabled[5], sub_window_enabled[5];

    //$2130
    uint8  color_mask, colorsub_mask;
    bool   addsub_mode;
    bool   direct_color;

    //$2131
    bool   color_mode, color_halve;
    bool   color_enabled[6];

    //$2132
    uint8  color_r, color_g, color_b;
    uint16 color_rgb;

    //$2133
    //overscan and interlace are checked once per frame to
    //determine if entire frame should be interlaced/non-interlace
    //and overscan adjusted. therefore, the variables act sort of
    //like a buffer, but they do still affect internal rendering
    bool   mode7_extbg;
    bool   pseudo_hires;
    bool   overscan;
    uint16 scanlines;
    bool   oam_interlace;
    bool   interlace;

    //$2137
    uint16 hcounter, vcounter;
    bool   latch_hcounter, latch_vcounter;
    bool   counters_latched;

    //$2139-$213a
    uint16 vram_readbuffer;

    //$213e
    bool   time_over, range_over;
    uint16 oam_itemcount, oam_tilecount;
  } regs;

  auto set_frameskip(uint frameskip) -> void;

  enum : uint { NTSC = 0, PAL = 1 };
  enum : uint { BG1 = 0, BG2 = 1, BG3 = 2, BG4 = 3, OAM = 4, BACK = 5, COL = 5 };
  enum : uint { SC_32x32 = 0, SC_64x32 = 1, SC_32x64 = 2, SC_64x64 = 3 };

  uint8 region;
  uint line;

  struct {
    //$2101
    uint8  oam_basesize;
    uint8  oam_nameselect;
    uint16 oam_tdaddr;

    //$210d-$210e
    uint16 m7_hofs, m7_vofs;

    //$211b-$2120
    uint16 m7a, m7b, m7c, m7d, m7x, m7y;
  } cache;

  uint16 mosaic_table[16][4096];
  uint frameskip;
  uint framecounter;

  #include "render/render.hpp"

  friend class Video;
};

extern PPU ppu;
