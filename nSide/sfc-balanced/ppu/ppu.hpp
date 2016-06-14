struct PPU : Thread, PPUcounter {
  alwaysinline auto interlace() const -> bool { return display.interlace; }
  alwaysinline auto overscan() const -> bool { return display.overscan; }
  alwaysinline auto vdisp() const -> uint { return display.overscan ? 240 : 225; }

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
  alwaysinline auto vramRead(uint addr) -> uint8;
  alwaysinline auto vramWrite(uint addr, uint8 data) -> void;
  alwaysinline auto oamRead(uint addr) -> uint8;
  alwaysinline auto oamWrite(uint addr, uint8 data) -> void;
  alwaysinline auto cgramRead(uint addr) -> uint8;
  alwaysinline auto cgramWrite(uint addr, uint8 data) -> void;

  //mmio.cpp
  auto read(uint24 addr, uint8 data) -> uint8;
  auto write(uint24 addr, uint8 data) -> void;
  auto latchCounters() -> void;
  auto updateVideoMode() -> void;

  struct {
    uint8 vram[128 * 1024];
    uint8 oam[544];
    uint8 cgram[512];
  } memory;

  //debugger functions
  auto exportRegisters(string &markup) -> void;

privileged:
  uint32* output = nullptr;

  struct {
    bool interlace;
    bool overscan;
  } display;

  alwaysinline auto addClocks(uint) -> void;

  auto scanline() -> void;
  auto renderScanline() -> void;
  auto frame() -> void;
  auto refresh() -> void;

  struct {
    uint version;
    uint8 mdr;
  } ppu1, ppu2;

  auto renderLine() -> void;
  auto update_oam_status() -> void;

  struct Latches {
    uint16 vram;
    uint8 oam;
    uint8 cgram;
    uint8 bgofs;
    uint8 mode7;
    bool counters;
    bool hcounter;
    bool vcounter;

    uint10 oamAddress;
    uint9 cgramAddress;
  } latch;

  struct Registers {
    uint16 bg_y[4];

    //$2100  INIDSP
    bool displayDisable;
    uint4 displayBrightness;

    //$2102  OAMADDL
    //$2103  OAMADDH
    uint10 oamBaseAddress;
    uint10 oamAddress;
    bool oamPriority;

    //$2105  BGMODE
    bool bgPriority;
    uint8 bgMode;

    //$2106  MOSAIC
    uint8  mosaicSize;
    uint16 mosaicCountdown;

    //$210d  BG1HOFS
    uint16 hoffsetMode7;

    //$210e  BG1VOFS
    uint16 voffsetMode7;

    //$2115  VMAIN
    bool vramIncrementMode;
    uint8 vramMapping;
    uint8 vramIncrementSize;

    //$2116  VMADDL
    //$2117  VMADDH
    uint16 vramAddress;

    //$211a  M7SEL
    uint8 repeatMode7;
    bool vflipMode7;
    bool hflipMode7;

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
    uint9 cgramAddress;

    //$2132  COLDATA
    uint16 color_rgb;

    //$2133  SETINI
    //overscan and interlace are checked once per frame to
    //determine if entire frame should be interlaced/non-interlace
    //and overscan adjusted. therefore, the variables act sort of
    //like a buffer, but they do still affect internal rendering
    bool extbg;
    bool pseudoHires;
    bool overscan;
    bool interlace;

    //$213c  OPHCT
    uint16 hcounter;

    //$213d  OPVCT
    uint16 vcounter;
  } r;

  uint line;

  struct {
    //$2101
    uint8  oam_baseSize;
    uint8  oam_nameSelect;
    uint16 oam_tiledataAddress;

    //$210d-$210e
    uint16 hoffsetMode7, voffsetMode7;

    //$211b-$2120
    uint16 m7a, m7b, m7c, m7d, m7x, m7y;
  } cache;

  uint16 mosaicTable[16][4096];

  #include "background/background.hpp"
  #include "screen/screen.hpp"
  #include "sprite/sprite.hpp"
  #include "window/window.hpp"
  #include "render/render.hpp"

  Background bg1;
  Background bg2;
  Background bg3;
  Background bg4;
  OAM oam;
  Window window;
  Screen screen;

  friend class PPU::Background;
  friend class PPU::OAM;
  friend class PPU::Window;
  friend class PPU::Screen;
  friend class Scheduler;
};

extern PPU ppu;
