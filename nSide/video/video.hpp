#pragma once

namespace Emulator {

struct Interface;
struct Video;
struct Sprite;

struct Video {
  enum class Effect : uint {
    ColorBleed,
    InterframeBlending,
    Scanlines,
    Rotation,
  };

  ~Video();

  auto reset() -> void;
  auto setInterface(Interface* interface) -> void;

  auto setPalette() -> void;
  auto setSaturation(double saturation) -> void;
  auto setGamma(double gamma) -> void;
  auto setLuminance(double luminance) -> void;

  auto setEffect(Effect effect, const any& value) -> void;

  auto createSprite(uint width, uint height) -> shared_pointer<Sprite>;
  auto removeSprite(shared_pointer<Sprite> sprite) -> bool;

  auto refresh(uint32* input, uint pitch, uint width, uint height) -> void;
  auto resize(uint width, uint height) -> void;
  auto refreshRegion(uint32* input, uint pitch, uint origin_x, uint origin_y, uint width, uint height, uint paletteOffset = 0) -> void;
  auto clear() -> void;

  auto screenshot(string path) -> void;

private:
  Interface* interface = nullptr;
  vector<shared_pointer<Sprite>> sprites;

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

  friend class Sprite;
};

struct Sprite {
  Sprite(uint width, uint height);
  ~Sprite();

  auto setPixels(const nall::image& image) -> void;
  auto setVisible(bool visible) -> void;
  auto setPosition(int x, int y) -> void;

private:
  const uint width;
  const uint height;
  uint32* pixels = nullptr;

  bool visible = false;
  int x = 0;
  int y = 0;

  friend class Video;
};

extern Video video;

}
