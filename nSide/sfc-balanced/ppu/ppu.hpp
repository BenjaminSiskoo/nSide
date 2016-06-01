struct PPU : Thread, PPUcounter {
  alwaysinline auto interlace() const -> bool { return display.interlace; }
  alwaysinline auto overscan() const -> bool { return display.overscan; }
  alwaysinline auto vdisp() const -> uint { return !display.overscan ? 225 : 240; }

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
  auto refresh() -> void;

  auto render_line() -> void;
  auto update_oam_status() -> void;

  struct Registers {
    uint8 ppu1_mdr;
    uint8 ppu2_mdr;

    uint16 vram_readbuffer;
    uint8 oam_latchdata;
    uint8 cgram_latchdata;
    uint8 bgofs_latchdata;
    uint8 mode7_latchdata;
    bool counters_latched;
    bool latch_hcounter;
    bool latch_vcounter;
    uint16 bg_y[4];

    uint16 oam_iaddr;
    uint16 cgram_iaddr;

    //$2100  INIDSP
    bool display_disable;
    uint8 display_brightness;

    //$2102  OAMADDL
    //$2103  OAMADDH
    uint16 oam_baseaddr;
    uint16 oam_addr;
    bool oam_priority;

    //$2105  BGMODE
    bool bg3_priority;
    uint8 bgmode;

    //$2106  MOSAIC
    uint8  mosaic_size;
    uint16 mosaic_countdown;

    //$210d  BG1HOFS
    uint16 mode7_hoffset;

    //$210e  BG1VOFS
    uint16 mode7_voffset;

    //$2115  VMAIN
    bool vram_incmode;
    uint8 vram_mapping;
    uint8 vram_incsize;

    //$2116  VMADDL
    //$2117  VMADDH
    uint16 vram_addr;

    //$211a  M7SEL
    uint8 mode7_repeat;
    bool mode7_vflip;
    bool mode7_hflip;

    //$211b  M7A
    uint16 m7a;

    //$211c  M7B
    uint16 m7b;

    //$211d  M7C
    uint16 m7c;

    //$211e  M7D
    uint16 m7d;

    //$211f  M7X
    uint16 m7x;

    //$2120  M7Y
    uint16 m7y;

    //$2121  CGADD
    uint16 cgram_addr;

    //$2132  COLDATA
    uint16 color_rgb;

    //$2133  SETINI
    //overscan and interlace are checked once per frame to
    //determine if entire frame should be interlaced/non-interlace
    //and overscan adjusted. therefore, the variables act sort of
    //like a buffer, but they do still affect internal rendering
    bool mode7_extbg;
    bool pseudo_hires;
    bool overscan;
    bool interlace;

    //$213c  OPHCT
    uint16 hcounter;

    //$213d  OPVCT
    uint16 vcounter;
  } regs;

  auto set_frameskip(uint frameskip) -> void;

  enum : uint { OAM = 4, BACK = 5, COL = 5 };

  uint line;

  struct {
    //$2101
    uint8  oam_basesize;
    uint8  oam_nameselect;
    uint16 oam_tdaddr;

    //$210d-$210e
    uint16 mode7_hoffset, mode7_voffset;

    //$211b-$2120
    uint16 m7a, m7b, m7c, m7d, m7x, m7y;
  } cache;

  uint16 mosaic_table[16][4096];
  uint frameskip;
  uint framecounter;

  #include "background/background.hpp"
  #include "screen/screen.hpp"
  #include "sprite/sprite.hpp"
  #include "window/window.hpp"
  #include "render/render.hpp"

  Background bg1;
  Background bg2;
  Background bg3;
  Background bg4;
  Sprite sprite;
  Window window;
  Screen screen;

  friend class PPU::Background;
  friend class PPU::Sprite;
  friend class PPU::Window;
  friend class PPU::Screen;
  friend class Scheduler;
};

extern PPU ppu;
