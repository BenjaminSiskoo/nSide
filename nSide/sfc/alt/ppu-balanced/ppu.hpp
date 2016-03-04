#include "../../ppu/video.hpp"

struct PPU : Thread, public PPUcounter {
  enum : bool { Threaded = true };

  PPU();
  ~PPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeCPU() -> void;

  #include "memory/memory.hpp"
  #include "mmio/mmio.hpp"
  #include "render/render.hpp"

  static auto Enter() -> void;
  auto add_clocks(uint clocks) -> void;

  alwaysinline auto interlace() const -> bool { return display.interlace; }
  alwaysinline auto overscan()  const -> bool { return display.overscan;  }

  auto render_line() -> void;
  auto update_oam_status() -> void;

  auto scanline() -> void;
  auto render_scanline() -> void;
  auto frame() -> void;

  auto main() -> void;
  auto enable() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto layer_enable(uint layer, uint priority, bool enable) -> void;
  auto set_frameskip(uint frameskip) -> void;

  auto serialize(serializer&) -> void;

  enum : uint { NTSC = 0, PAL = 1 };
  enum : uint { BG1 = 0, BG2 = 1, BG3 = 2, BG4 = 3, OAM = 4, BACK = 5, COL = 5 };
  enum : uint { SC_32x32 = 0, SC_64x32 = 1, SC_32x64 = 2, SC_64x64 = 3 };

  uint8_t vram[128 * 1024];
  uint8_t oam[544];
  uint8_t cgram[512];

  uint32_t* output;

  uint ppu1_version = 1;
  uint ppu2_version = 3;

  uint8_t region;
  uint line;

  struct {
    bool interlace;
    bool overscan;
  } display;

  struct {
    //$2101
    uint8_t  oam_basesize;
    uint8_t  oam_nameselect;
    uint16_t oam_tdaddr;

    //$210d-$210e
    uint16_t m7_hofs, m7_vofs;

    //$211b-$2120
    uint16_t m7a, m7b, m7c, m7d, m7x, m7y;
  } cache;

  uint16_t mosaic_table[16][4096];
  bool layer_enabled[5][4];
  uint frameskip;
  uint framecounter;

  friend class Video;

  //debugger functions
  auto exportRegisters(string &markup) -> void;
};

extern PPU ppu;