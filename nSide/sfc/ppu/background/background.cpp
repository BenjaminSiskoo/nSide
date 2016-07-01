#include "mode7.cpp"

auto PPU::Background::voffset() const -> uint16 {
  if(io.mosaic) return latch.voffset;
  return io.voffset;
}

auto PPU::Background::hoffset() const -> uint16 {
  if(io.mosaic) return latch.hoffset;
  return io.hoffset;
}

//V = 0, H = 0
auto PPU::Background::frame() -> void {
}

//H = 0
auto PPU::Background::scanline() -> void {
}

//H = 28
auto PPU::Background::begin() -> void {
  bool hires = (ppu.io.bgMode == 5 || ppu.io.bgMode == 6);
  x = -7;
  y = ppu.vcounter();

  if(y == 1) {
    mosaic.vcounter = io.mosaic + 1;
    mosaic.voffset = 1;
    latch.hoffset = io.hoffset;
    latch.voffset = io.voffset;
  } else if(--mosaic.vcounter == 0) {
    mosaic.vcounter = io.mosaic + 1;
    mosaic.voffset += io.mosaic + 1;
    latch.hoffset = io.hoffset;
    latch.voffset = io.voffset;
  }

  tileCounter = (7 - (latch.hoffset & 7)) << hires;
  for(auto& d : data) d = 0;

  mosaic.hcounter = io.mosaic + 1;
  mosaic.hoffset = 0;

  if(io.mode == Mode::Mode7) return beginMode7();
  if(io.mosaic == 0) {
    latch.hoffset = io.hoffset;
    latch.voffset = io.voffset;
  }
}

auto PPU::Background::getTile() -> void {
  bool hires = (ppu.io.bgMode == 5 || ppu.io.bgMode == 6);

  uint colorDepth = (io.mode == Mode::BPP2 ? 0 : io.mode == Mode::BPP4 ? 1 : 2);
  uint paletteOffset = (ppu.io.bgMode == 0 ? id << 5 : 0);
  uint paletteSize = 2 << colorDepth;
  uint tileMask = ppu.vram.mask >> (3 + colorDepth);
  uint tiledataIndex = io.tiledataAddress >> (3 + colorDepth);

  uint tileHeight = (io.tileSize == TileSize::Size8x8 ? 3 : 4);
  uint tileWidth = (!hires ? tileHeight : 4);

  uint width = 256 << hires;

  uint hmask = (tileHeight == 3 ? width : width << 1);
  uint vmask = hmask;
  if(io.screenSize & 1) hmask <<= 1;
  if(io.screenSize & 2) vmask <<= 1;
  hmask--;
  vmask--;

  uint px = x << hires;
  uint py = (io.mosaic == 0 ? y : mosaic.voffset);

  uint hscroll = hoffset();
  uint vscroll = voffset();
  if(hires) {
    hscroll <<= 1;
    if(ppu.io.interlace) py = (py << 1) + ppu.field();
  }

  uint hoffset = hscroll + px;
  uint voffset = vscroll + py;

  if(ppu.io.bgMode == 2 || ppu.io.bgMode == 4 || ppu.io.bgMode == 6) {
    uint16 offsetX = (x + (hscroll & 7));

    if(offsetX >= 8) {
      uint hval = ppu.bg3.getTile((offsetX - 8) + (ppu.bg3.hoffset() & ~7), ppu.bg3.voffset() + 0);
      uint vval = ppu.bg3.getTile((offsetX - 8) + (ppu.bg3.hoffset() & ~7), ppu.bg3.voffset() + 8);
      uint validMask = (id == ID::BG1 ? 0x2000 : 0x4000);

      if(ppu.io.bgMode == 4) {
        if(hval & validMask) {
          if((hval & 0x8000) == 0) {
            hoffset = offsetX + (hval & ~7);
          } else {
            voffset = y + hval;
          }
        }
      } else {
        if(hval & validMask) hoffset = offsetX + (hval & ~7);
        if(vval & validMask) voffset = y + vval;
      }
    }
  }

  hoffset &= hmask;
  voffset &= vmask;

  uint screenX = (io.screenSize & 1 ? 32 << 5 : 0);
  uint screenY = (io.screenSize & 2 ? 32 << 5 : 0);
  if(io.screenSize == 3) screenY <<= 1;

  uint tx = hoffset >> tileWidth;
  uint ty = voffset >> tileHeight;

  uint16 offset = ((ty & 0x1f) << 5) + (tx & 0x1f);
  if(tx & 0x20) offset += screenX;
  if(ty & 0x20) offset += screenY;

  uint16 address = io.screenAddress + offset;
  tile = ppu.vram[address];
  bool mirrorY = tile & 0x8000;
  bool mirrorX = tile & 0x4000;
  priority = io.priority[bool(tile & 0x2000)];
  paletteNumber = (tile >> 10) & 7;
  paletteIndex = paletteOffset + (paletteNumber << paletteSize);

  if(tileWidth   == 4 && (bool)(hoffset & 8) != mirrorX) tile +=  1;
  if(tileHeight  == 4 && (bool)(voffset & 8) != mirrorY) tile += 16;
  uint16 character = ((tile & 0x03ff) + tiledataIndex) & tileMask;

  if(mirrorY) voffset ^= 7;
  offset = (character << (3 + colorDepth)) + (voffset & 7);

  switch(io.mode) {
  case Mode::BPP8:
    data[1].bits(16,31) = ppu.vram[offset + 24];
    data[1].bits( 0,15) = ppu.vram[offset + 16];
  case Mode::BPP4:
    data[0].bits(16,31) = ppu.vram[offset +  8];
  case Mode::BPP2:
    data[0].bits( 0,15) = ppu.vram[offset +  0];
  }

  if(mirrorX) for(auto n : range(2)) {
    data[n] = ((data[n] >> 4) & 0x0f0f0f0f) | ((data[n] << 4) & 0xf0f0f0f0);
    data[n] = ((data[n] >> 2) & 0x33333333) | ((data[n] << 2) & 0xcccccccc);
    data[n] = ((data[n] >> 1) & 0x55555555) | ((data[n] << 1) & 0xaaaaaaaa);
  }
}

auto PPU::Background::run(bool screen) -> void {
  if(ppu.vcounter() == 0) return;
  bool hires = (ppu.io.bgMode == 5 || ppu.io.bgMode == 6);

  if(screen == Screen::Below) {
    output.above.priority = 0;
    output.below.priority = 0;
    if(!hires) return;
  }

  if(tileCounter-- == 0) {
    tileCounter = 7;
    getTile();
  }

  if(io.mode == Mode::Mode7) return runMode7();

  uint8 palette = getTileColor();
  if(x == 0) mosaic.hcounter = 1;
  if(x >= 0 && --mosaic.hcounter == 0) {
    mosaic.hcounter = io.mosaic + 1;
    mosaic.priority = priority;
    mosaic.palette = palette ? paletteIndex + palette : 0;
    mosaic.tile = tile;
  }
  if(screen == Screen::Above) x++;
  if(mosaic.palette == 0) return;

  if(!hires || screen == Screen::Above) if(io.aboveEnable) output.above = mosaic;
  if(!hires || screen == Screen::Below) if(io.belowEnable) output.below = mosaic;
}

auto PPU::Background::getTileColor() -> uint {
  uint color = 0;

  switch(io.mode) {
  case Mode::BPP8:
    color += data[1] >> 24 & 0x80;
    color += data[1] >> 17 & 0x40;
    color += data[1] >> 10 & 0x20;
    color += data[1] >>  3 & 0x10;
    data[1] <<= 1;
  case Mode::BPP4:
    color += data[0] >> 28 & 0x08;
    color += data[0] >> 21 & 0x04;
  case Mode::BPP2:
    color += data[0] >> 14 & 0x02;
    color += data[0] >>  7 & 0x01;
    data[0] <<= 1;
  }

  return color;
}

auto PPU::Background::reset() -> void {
  io.tiledataAddress = (random(0x0000) & 0x0f) << 12;
  io.screenAddress = (random(0x0000) & 0xfc) << 8;
  io.screenSize = random(0);
  io.mosaic = random(0);
  io.tileSize = random(0);
  io.mode = 0;
  for(auto& p : io.priority) p = 0;
  io.aboveEnable = random(0);
  io.belowEnable = random(0);
  io.hoffset = random(0x0000);
  io.voffset = random(0x0000);

  latch.hoffset = 0;
  latch.voffset = 0;

  output.above.palette = 0;
  output.above.priority = 0;
  output.below.palette = 0;
  output.below.priority = 0;

  mosaic.priority = 0;
  mosaic.palette = 0;
  mosaic.tile = 0;

  mosaic.vcounter = 0;
  mosaic.voffset = 0;
  mosaic.hcounter = 0;
  mosaic.hoffset = 0;

  x = 0;
  y = 0;

  tileCounter = 0;
  tile = 0;
  priority = 0;
  paletteNumber = 0;
  paletteIndex = 0;
  for(auto& d : data) d = 0;
}

auto PPU::Background::getTile(uint x, uint y) -> uint {
  bool hires = (ppu.io.bgMode == 5 || ppu.io.bgMode == 6);
  uint tileHeight = (io.tileSize == TileSize::Size8x8 ? 3 : 4);
  uint tileWidth = (!hires ? tileHeight : 4);
  uint width = (!hires ? 256 : 512);
  uint maskX = (tileHeight == 3 ? width : width << 1);
  uint maskY = maskX;
  if(io.screenSize & 1) maskX <<= 1;
  if(io.screenSize & 2) maskY <<= 1;
  maskX--;
  maskY--;

  uint screenX = (io.screenSize & 1 ? 32 << 5 : 0);
  uint screenY = (io.screenSize & 2 ? 32 << 5 : 0);
  if(io.screenSize == 3) screenY <<= 1;

  x = (x & maskX) >> tileWidth;
  y = (y & maskY) >> tileHeight;

  uint16 offset = ((y & 0x1f) << 5) + (x & 0x1f);
  if(x & 0x20) offset += screenX;
  if(y & 0x20) offset += screenY;

  uint16 address = io.screenAddress + offset;
  return ppu.vram[address];
}
