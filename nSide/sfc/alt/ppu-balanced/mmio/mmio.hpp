struct {
  //open bus support
  uint8_t ppu1_mdr, ppu2_mdr;

  //bg line counters
  uint16_t bg_y[4];

  //internal state
  uint16_t ioamaddr;
  uint16_t icgramaddr;

  //$2100
  bool   display_disabled;
  uint8_t  display_brightness;

  //$2101
  uint8_t  oam_basesize;
  uint8_t  oam_nameselect;
  uint16_t oam_tdaddr;

  //$2102-$2103
  uint16_t oam_baseaddr;
  uint16_t oam_addr;
  bool   oam_priority;
  uint8_t  oam_firstsprite;

  //$2104
  uint8_t  oam_latchdata;

  //$2105
  bool   bg_tilesize[4];
  bool   bg3_priority;
  uint8_t  bg_mode;

  //$2106
  uint8_t  mosaic_size;
  bool   mosaic_enabled[4];
  uint16_t mosaic_countdown;

  //$2107-$210a
  uint16_t bg_scaddr[4];
  uint8_t  bg_scsize[4];

  //$210b-$210c
  uint16_t bg_tdaddr[4];

  //$210d-$2114
  uint8_t  bg_ofslatch;
  uint16_t m7_hofs, m7_vofs;
  uint16_t bg_hofs[4];
  uint16_t bg_vofs[4];

  //$2115
  bool   vram_incmode;
  uint8_t  vram_mapping;
  uint8_t  vram_incsize;

  //$2116-$2117
  uint16_t vram_addr;

  //$211a
  uint8_t  mode7_repeat;
  bool   mode7_vflip;
  bool   mode7_hflip;

  //$211b-$2120
  uint8_t  m7_latch;
  uint16_t m7a, m7b, m7c, m7d, m7x, m7y;

  //$2121
  uint16_t cgram_addr;

  //$2122
  uint8_t  cgram_latchdata;

  //$2123-$2125
  bool   window1_enabled[6];
  bool   window1_invert [6];
  bool   window2_enabled[6];
  bool   window2_invert [6];

  //$2126-$2129
  uint8_t  window1_left, window1_right;
  uint8_t  window2_left, window2_right;

  //$212a-$212b
  uint8_t  window_mask[6];

  //$212c-$212d
  bool   bg_enabled[5], bgsub_enabled[5];

  //$212e-$212f
  bool   window_enabled[5], sub_window_enabled[5];

  //$2130
  uint8_t  color_mask, colorsub_mask;
  bool   addsub_mode;
  bool   direct_color;

  //$2131
  bool   color_mode, color_halve;
  bool   color_enabled[6];

  //$2132
  uint8_t  color_r, color_g, color_b;
  uint16_t color_rgb;

  //$2133
  //overscan and interlace are checked once per frame to
  //determine if entire frame should be interlaced/non-interlace
  //and overscan adjusted. therefore, the variables act sort of
  //like a buffer, but they do still affect internal rendering
  bool   mode7_extbg;
  bool   pseudo_hires;
  bool   overscan;
  uint16_t scanlines;
  bool   oam_interlace;
  bool   interlace;

  //$2137
  uint16_t hcounter, vcounter;
  bool   latch_hcounter, latch_vcounter;
  bool   counters_latched;

  //$2139-$213a
  uint16_t vram_readbuffer;

  //$213e
  bool   time_over, range_over;
  uint16_t oam_itemcount, oam_tilecount;
} regs;

auto mmio_w2100(uint8_t value) -> void;  //INIDISP
auto mmio_w2101(uint8_t value) -> void;  //OBSEL
auto mmio_w2102(uint8_t value) -> void;  //OAMADDL
auto mmio_w2103(uint8_t value) -> void;  //OAMADDH
auto mmio_w2104(uint8_t value) -> void;  //OAMDATA
auto mmio_w2105(uint8_t value) -> void;  //BGMODE
auto mmio_w2106(uint8_t value) -> void;  //MOSAIC
auto mmio_w2107(uint8_t value) -> void;  //BG1SC
auto mmio_w2108(uint8_t value) -> void;  //BG2SC
auto mmio_w2109(uint8_t value) -> void;  //BG3SC
auto mmio_w210a(uint8_t value) -> void;  //BG4SC
auto mmio_w210b(uint8_t value) -> void;  //BG12NBA
auto mmio_w210c(uint8_t value) -> void;  //BG34NBA
auto mmio_w210d(uint8_t value) -> void;  //BG1HOFS
auto mmio_w210e(uint8_t value) -> void;  //BG1VOFS
auto mmio_w210f(uint8_t value) -> void;  //BG2HOFS
auto mmio_w2110(uint8_t value) -> void;  //BG2VOFS
auto mmio_w2111(uint8_t value) -> void;  //BG3HOFS
auto mmio_w2112(uint8_t value) -> void;  //BG3VOFS
auto mmio_w2113(uint8_t value) -> void;  //BG4HOFS
auto mmio_w2114(uint8_t value) -> void;  //BG4VOFS
auto mmio_w2115(uint8_t value) -> void;  //VMAIN
auto mmio_w2116(uint8_t value) -> void;  //VMADDL
auto mmio_w2117(uint8_t value) -> void;  //VMADDH
auto mmio_w2118(uint8_t value) -> void;  //VMDATAL
auto mmio_w2119(uint8_t value) -> void;  //VMDATAH
auto mmio_w211a(uint8_t value) -> void;  //M7SEL
auto mmio_w211b(uint8_t value) -> void;  //M7A
auto mmio_w211c(uint8_t value) -> void;  //M7B
auto mmio_w211d(uint8_t value) -> void;  //M7C
auto mmio_w211e(uint8_t value) -> void;  //M7D
auto mmio_w211f(uint8_t value) -> void;  //M7X
auto mmio_w2120(uint8_t value) -> void;  //M7Y
auto mmio_w2121(uint8_t value) -> void;  //CGADD
auto mmio_w2122(uint8_t value) -> void;  //CGDATA
auto mmio_w2123(uint8_t value) -> void;  //W12SEL
auto mmio_w2124(uint8_t value) -> void;  //W34SEL
auto mmio_w2125(uint8_t value) -> void;  //WOBJSEL
auto mmio_w2126(uint8_t value) -> void;  //WH0
auto mmio_w2127(uint8_t value) -> void;  //WH1
auto mmio_w2128(uint8_t value) -> void;  //WH2
auto mmio_w2129(uint8_t value) -> void;  //WH3
auto mmio_w212a(uint8_t value) -> void;  //WBGLOG
auto mmio_w212b(uint8_t value) -> void;  //WOBJLOG
auto mmio_w212c(uint8_t value) -> void;  //TM
auto mmio_w212d(uint8_t value) -> void;  //TS
auto mmio_w212e(uint8_t value) -> void;  //TMW
auto mmio_w212f(uint8_t value) -> void;  //TSW
auto mmio_w2130(uint8_t value) -> void;  //CGWSEL
auto mmio_w2131(uint8_t value) -> void;  //CGADDSUB
auto mmio_w2132(uint8_t value) -> void;  //COLDATA
auto mmio_w2133(uint8_t value) -> void;  //SETINI

auto mmio_r2134() -> uint8_t;  //MPYL
auto mmio_r2135() -> uint8_t;  //MPYM
auto mmio_r2136() -> uint8_t;  //MPYH
auto mmio_r2137() -> uint8_t;  //SLHV
auto mmio_r2138() -> uint8_t;  //OAMDATAREAD
auto mmio_r2139() -> uint8_t;  //VMDATALREAD
auto mmio_r213a() -> uint8_t;  //VMDATAHREAD
auto mmio_r213b() -> uint8_t;  //CGDATAREAD
auto mmio_r213c() -> uint8_t;  //OPHCT
auto mmio_r213d() -> uint8_t;  //OPVCT
auto mmio_r213e() -> uint8_t;  //STAT77
auto mmio_r213f() -> uint8_t;  //STAT78

auto mmio_read(uint_t addr, uint8_t data) -> uint8_t;
auto mmio_write(uint_t addr, uint8_t data) -> void;

auto latch_counters() -> void;
