#include "../../ppu/video.hpp"

struct PPU : Thread, public PPUcounter {
  enum : bool { Threaded = true };

  PPU();
  ~PPU();

  alwaysinline auto step(uint_t clocks) -> void;
  alwaysinline auto synchronizeCPU() -> void;

  auto latch_counters() -> void;
  auto interlace() const -> bool;
  auto overscan() const -> bool;

  auto main() -> void;
  auto enable() -> void;
  auto power() -> void;
  auto reset() -> void;
  auto scanline() -> void;
  auto frame() -> void;

  auto layer_enable(uint_t layer, uint_t priority, bool enable) -> void;
  auto set_frameskip(uint_t frameskip) -> void;

  auto serialize(serializer&) -> void;

  uint8_t vram[64 * 1024];
  uint8_t oam[544];
  uint8_t cgram[512];

  //debugger functions
  auto exportRegisters(string &markup) -> void;

private:
  uint32_t* output;

  #include "mmio/mmio.hpp"
  #include "window/window.hpp"
  #include "cache/cache.hpp"
  #include "background/background.hpp"
  #include "sprite/sprite.hpp"
  #include "screen/screen.hpp"

  Cache cache;
  Background bg1;
  Background bg2;
  Background bg3;
  Background bg4;
  Sprite sprite;
  Screen screen;

  struct Display {
    bool interlace;
    bool overscan;
    uint_t width;
    uint_t height;
    uint_t frameskip;
    uint_t framecounter;
  } display;

  static auto Enter() -> void;
  auto add_clocks(uint_t clocks) -> void;
  auto render_scanline() -> void;

  friend class PPU::Cache;
  friend class PPU::Background;
  friend class PPU::Sprite;
  friend class PPU::Screen;
  friend class Video;
};

extern PPU ppu;
