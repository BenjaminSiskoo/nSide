#pragma once

namespace Emulator {

struct Interface;

struct Video {
  enum class Effect : uint {
    ColorBleed,
    InterframeBlending,
    Scanlines,
    Rotation,
  };

  struct Cursor {
    uint x = 0;
    uint y = 0;
    uint origin_x = 0;
    uint origin_y = 0;
    uint width = 0;
    uint height = 0;
    uint stretch_x = 1;
    uint stretch_y = 1;
    const uint64* palette = nullptr;
    const uint8* data = nullptr;
  };

  ~Video();

  auto reset() -> void;
  auto setInterface(Interface* interface) -> void;

  auto setPalette() -> void;
  auto setSaturation(double saturation) -> void;
  auto setGamma(double gamma) -> void;
  auto setLuminance(double luminance) -> void;

  auto setEffect(Effect effect, const any& value) -> void;
  auto addCursor(Cursor& cursor) -> void;
  auto clearCursors() -> void;

  auto refresh(uint32* input, uint pitch, uint width, uint height) -> void;
  auto resize(uint width, uint height) -> void;
  auto refreshRegion(uint32* input, uint pitch, uint origin_x, uint origin_y, uint width, uint height, uint paletteOffset = 0) -> void;
  auto drawCursors() -> void;
  auto clear() -> void;

private:
  Emulator::Interface* interface = nullptr;
  uint32* output = nullptr;
  uint32* palette = nullptr;
  uint width = 0;
  uint height = 0;
  uint colors = 0;

  double saturation = 1.0;
  double gamma = 1.0;
  double luminance = 1.0;

  struct Effects {
    bool colorBleed = false;
    bool interframeBlending = false;
    bool scanlines = false;
    uint2 rotation = 0;
  } effects;

  vector<Cursor*> cursors;
};

extern Video video;

}
