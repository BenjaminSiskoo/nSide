#include "../shimai.hpp"
#include "sprite.cpp"
unique_pointer<Graphics> graphics;

Graphics::Graphics() {
  graphics = this;
};

auto Graphics::initialize() -> void {
  buffer.allocate(854, 480);
  scale = buffer.height() / 240.0;
}

auto Graphics::reset() -> void {
  video->set(Video::Filter, settings["Video/Shader"].text() == "None" ? Video::FilterNearest : Video::FilterLinear);
  video->set(Video::Shader, (string)"");
  step = 0;

  sprites.reset();
  home->loadSprites(sprites);
}

auto Graphics::run() -> void {
  uint sampleRate = audio->get(Audio::Frequency).get<uint>(44100) / 60;

  if(step == 0) {
    home->updateSprites();
  }

  if(step == 1) {
    buffer.fill(theme->backgroundColor);
  }

  if(step >= 2 && step < sprites.size() + 2) {
    auto sprite = sprites[step - 2];
    uint x = max(0, sprite->x - sprite->ox);
    uint y = max(0, sprite->y - sprite->oy);
    if(x < buffer.width() && y < buffer.height()) {
      uint rx = x - (sprite->x - sprite->ox);
      uint ry = y - (sprite->y - sprite->oy);
      if(rx < sprite->img->width() && ry < sprite->img->height()) {
        uint rw = min(sprite->img->width () - rx, buffer.width () - x);
        uint rh = min(sprite->img->height() - ry, buffer.height() - y);
        buffer.impose(image::blend::sourceAlpha, x, y, *sprite->img, rx, ry, rw, rh);
      }
    }
  }

  if(step == sampleRate - 1) {
    auto output = (uint32*)buffer.data();
    uint width  = buffer.width();
    uint height = buffer.height();
    program->videoRefresh(output, width * sizeof(uint32), width, height);
  }

  if(step++ >= sampleRate) step = 0;
}

auto Graphics::drawTextCenter(string_view text, image& img) -> void {
  img.fill(0x00000000);

  vector<uint32_t> codepoints;
  uint32_t codepoint;
  uint remainingBytes = 0;
  uint byteIndex = 0;
  while(byteIndex < text.size()) {
    if(remainingBytes) {
      remainingBytes--;
      codepoint |= (text[byteIndex++] & 0x3f) << (remainingBytes * 6);
      if(remainingBytes) continue;
      codepoints.append(codepoint);
      codepoint = 0;
    } else if(text[byteIndex] & 0x80) {
      if((text[byteIndex] & 0xe0) == 0xc0) remainingBytes = 1;
      if((text[byteIndex] & 0xf0) == 0xe0) remainingBytes = 2;
      if((text[byteIndex] & 0xf8) == 0xf0) remainingBytes = 3;
      codepoint = (text[byteIndex++] & (0x3f >> remainingBytes)) << (remainingBytes * 6);
    } else {
      codepoints.append(text[byteIndex++]);
    }
  }

  uint width = 0;
  for(uint index : range(codepoints.size())) {
    uint32_t codepoint = codepoints[index];
    auto glyph = theme->font.u[codepoint];
    if(!glyph) continue;
    width += glyph->width();
  }

  uint x = (img.width() - width) / 2;
  for(uint index : range(codepoints.size())) {
    uint32_t codepoint = codepoints[index];
    auto glyph = theme->font.u[codepoint];
    if(!glyph) continue;
    uint w = glyph->width();
    uint h = glyph->height();
    uint y = (img.height() - h) / 2;
    img.impose(image::blend::sourceAlpha, x, y, *glyph, 0, 0, w, h);
    x += w;
  }
}
