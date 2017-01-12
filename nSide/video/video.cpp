#include <emulator/emulator.hpp>

namespace Emulator {

#include "sprite.cpp"
Video video;

Video::~Video() {
  reset();
}

auto Video::reset() -> void {
  interface = nullptr;
  sprites.reset();
  delete output;
  output = nullptr;
  delete palette;
  palette = nullptr;
  width = 0;
  height = 0;
  effects.colorBleed = false;
  effects.interframeBlending = false;
  effects.rotation = 0;
}

auto Video::setInterface(Interface* interface) -> void {
  this->interface = interface;
}

auto Video::setPalette() -> void {
  if(!interface) return;

  delete palette;
  colors = interface->videoColors();
  palette = new uint32[colors];
  for(auto index : range(colors)) {
    uint64 color = interface->videoColor(index);
    uint16 b = color.bits( 0,15);
    uint16 g = color.bits(16,31);
    uint16 r = color.bits(32,47);
    uint16 a = 0xffff;

    if(saturation != 1.0) {
      uint16 grayscale = uclamp<16>((r + g + b) / 3);
      double inverse = max(0.0, 1.0 - saturation);
      r = uclamp<16>(r * saturation + grayscale * inverse);
      g = uclamp<16>(g * saturation + grayscale * inverse);
      b = uclamp<16>(b * saturation + grayscale * inverse);
    }

    if(gamma != 1.0) {
      double reciprocal = 1.0 / 32767.0;
      r = r > 32767 ? r : uint16(32767 * pow(r * reciprocal, gamma));
      g = g > 32767 ? g : uint16(32767 * pow(g * reciprocal, gamma));
      b = b > 32767 ? b : uint16(32767 * pow(b * reciprocal, gamma));
    }

    if(luminance != 1.0) {
      r = uclamp<16>(r * luminance);
      g = uclamp<16>(g * luminance);
      b = uclamp<16>(b * luminance);
    }

    //convert color from 16-bits/channel to 8-bits/channel; force alpha to 1.0
    palette[index] = a.byte(1) << 24 | r.byte(1) << 16 | g.byte(1) << 8 | b.byte(1) << 0;
  }
}

auto Video::setSaturation(double saturation) -> void {
  this->saturation = saturation;
}

auto Video::setGamma(double gamma) -> void {
  this->gamma = gamma;
}

auto Video::setLuminance(double luminance) -> void {
  this->luminance = luminance;
}

auto Video::setEffect(Effect effect, const any& value) -> void {
  if(effect == Effect::ColorBleed && value.is<bool>()) {
    effects.colorBleed = value.get<bool>();
  }

  if(effect == Effect::InterframeBlending && value.is<bool>()) {
    effects.interframeBlending = value.get<bool>();
  }

  if(effect == Effect::Scanlines && value.is<bool>()) {
    effects.scanlines = value.get<bool>();
    resize(this->width, this->height);
  }

  if(effect == Effect::Rotation) {
    if(value.is<int>())   effects.rotation = value.get<int>();
    if(value.is<uint>())  effects.rotation = value.get<uint>();
    if(value.is<uint2>()) effects.rotation = value.get<uint2>();
  }
}

auto Video::createSprite(uint width, uint height) -> shared_pointer<Sprite> {
  shared_pointer<Sprite> sprite = new Sprite{width, height};
  sprites.append(sprite);
  return sprite;
}

auto Video::removeSprite(shared_pointer<Sprite> sprite) -> bool {
  for(uint n : range(sprites)) {
    if(sprite == sprites[n]) {
      sprites.remove(n);
      return true;
    }
  }
  return false;
}

auto Video::refresh(uint32* input, uint pitch, uint width, uint height) -> void {
  resize(width, height);
  refreshRegion(input, pitch, 0, 0, width, height);
}

auto Video::resize(uint width, uint height) -> void {
  if(this->width != width || this->height != height) {
    delete output;
    output = new uint32[width * (height << effects.scanlines)]();
    this->width = width;
    this->height = height;
  }
}

auto Video::refreshRegion(uint32* input, uint pitch, uint origin_x, uint origin_y, uint width, uint height, uint paletteOffset) -> void {
  pitch >>= 2;  //bytes to words

  if(!effects.scanlines) {
    int increment;
    switch(effects.rotation) {
    case 0: increment =  1;            break;
    case 1: increment =  this->height; break;
    case 2: increment = -1;            break;
    case 3: increment = -this->height; break;
    }
    for(uint y : range(height)) {
      uint32* source = input + y * pitch;
      uint32* target = output + (origin_y + y) * this->width + origin_x;
      switch(effects.rotation) {
      case 1: target = output + (this->height - 1 - origin_y - y); break;
      case 2: target = output + (this->height - 1 - origin_y - y) * this->width + this->width - 1 - origin_x; break;
      case 3: target = output + (this->width - 1 - origin_x) * this->height + origin_y + y; break;
      }

      if(!effects.interframeBlending) {
        for(uint x : range(width)) {
          auto color = palette[*source++ + paletteOffset];
          *target = color;
          target += increment;
        }
      } else {
        for(uint x : range(width)) {
          auto a = *target;
          auto b = palette[*source++ + paletteOffset];
          *target = (a + b - ((a ^ b) & 0x01010101)) >> 1;
          target += increment;
        }
      }
    }
  } else {
    for(uint y : range(height)) {
      auto source = input + y * pitch;
      auto targetLo = output + (origin_y + y) * this->width * 2 + origin_x;
      auto targetHi = output + (origin_y + y) * this->width * 2 + origin_x + this->width;

      if(!effects.interframeBlending) {
        for(uint x : range(width)) {
          auto color = palette[*source++ + paletteOffset];
          *targetLo++ = color;
          *targetHi++ = (255 << 24) | ((color & 0xfefefe) >> 1);
        }
      } else {
        for(uint x : range(width)) {
          auto a = *targetLo;
          auto b = palette[*source++ + paletteOffset];
          *targetLo++ = (a + b - ((a ^ b) & 0x01010101)) >> 1;
          auto c = *targetHi;
          auto d = (255 << 24) | ((b & 0xfefefe) >> 1);
          *targetHi++ = (c + d - ((c ^ d) & 0x01010101)) >> 1;
        }
      }
    }
  }

  if(effects.colorBleed) {
    for(uint y : range(height)) {
      auto target = output + (origin_y + y) * this->width + origin_x;
      for(uint x : range(width)) {
        auto a = target[x];
        auto b = target[x + (x != width - 1)];
        target[x] = (a + b - ((a ^ b) & 0x01010101)) >> 1;
      }
    }
  }

  for(auto& sprite : sprites) {
    if(!sprite->visible) continue;

    for(int y : range(sprite->height)) {
      for(int x : range(sprite->width)) {
        int pixelY = sprite->y + y;
        if(pixelY < 0 || pixelY >= height) continue;

        int pixelX = sprite->x + x;
        if(pixelX < 0 || pixelX >= width) continue;

        auto pixel = sprite->pixels[y * sprite->width + x];
        if(pixel) output[pixelY * width + pixelX] = 0xff000000 | pixel;
      }
    }
  }

  if(!effects.rotation.bit(0)) {
    platform->videoRefresh(output, this->width * sizeof(uint32), this->width, this->height << effects.scanlines);
  } else {
    platform->videoRefresh(
      output, (this->height << effects.scanlines) * sizeof(uint32),
      this->height << effects.scanlines, this->width
    );
  }
}

auto Video::clear() -> void {
  memory::fill(output, width * (height << effects.scanlines) * sizeof(uint32));
}

}
