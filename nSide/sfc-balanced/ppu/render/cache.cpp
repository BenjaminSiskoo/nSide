auto PPU::render_bg_tile(uint color_depth, uint16 tile_num) -> void {
  uint8 col, d0, d1, d2, d3, d4, d5, d6, d7;

  #define render_bg_tile_line_2bpp(mask) \
    col  = !!(d0 & mask) << 0; \
    col += !!(d1 & mask) << 1; \
    *dest++ = col

  #define render_bg_tile_line_4bpp(mask) \
    col  = !!(d0 & mask) << 0; \
    col += !!(d1 & mask) << 1; \
    col += !!(d2 & mask) << 2; \
    col += !!(d3 & mask) << 3; \
    *dest++ = col

  #define render_bg_tile_line_8bpp(mask) \
    col  = !!(d0 & mask) << 0; \
    col += !!(d1 & mask) << 1; \
    col += !!(d2 & mask) << 2; \
    col += !!(d3 & mask) << 3; \
    col += !!(d4 & mask) << 4; \
    col += !!(d5 & mask) << 5; \
    col += !!(d6 & mask) << 6; \
    col += !!(d7 & mask) << 7; \
    *dest++ = col

  if(color_depth == Background::Mode::BPP2) {
    uint8* dest = (uint8*)bg_tiledata[Background::Mode::BPP2] + tile_num * 64;
    uint pos = tile_num * 16;
    uint y = 8;
    while(y--) {
      d0 = memory.vram[pos + 0];
      d1 = memory.vram[pos + 1];
      render_bg_tile_line_2bpp(0x80);
      render_bg_tile_line_2bpp(0x40);
      render_bg_tile_line_2bpp(0x20);
      render_bg_tile_line_2bpp(0x10);
      render_bg_tile_line_2bpp(0x08);
      render_bg_tile_line_2bpp(0x04);
      render_bg_tile_line_2bpp(0x02);
      render_bg_tile_line_2bpp(0x01);
      pos += 2;
    }
    bg_tiledataState[Background::Mode::BPP2][tile_num] = 0;
  }

  if(color_depth == Background::Mode::BPP4) {
    uint8* dest = (uint8*)bg_tiledata[Background::Mode::BPP4] + tile_num * 64;
    uint pos = tile_num * 32;
    uint y = 8;
    while(y--) {
      d0 = memory.vram[pos +  0];
      d1 = memory.vram[pos +  1];
      d2 = memory.vram[pos + 16];
      d3 = memory.vram[pos + 17];
      render_bg_tile_line_4bpp(0x80);
      render_bg_tile_line_4bpp(0x40);
      render_bg_tile_line_4bpp(0x20);
      render_bg_tile_line_4bpp(0x10);
      render_bg_tile_line_4bpp(0x08);
      render_bg_tile_line_4bpp(0x04);
      render_bg_tile_line_4bpp(0x02);
      render_bg_tile_line_4bpp(0x01);
      pos += 2;
    }
    bg_tiledataState[Background::Mode::BPP4][tile_num] = 0;
  }

  if(color_depth == Background::Mode::BPP8) {
    uint8* dest = (uint8*)bg_tiledata[Background::Mode::BPP8] + tile_num * 64;
    uint pos = tile_num * 64;
    uint y = 8;
    while(y--) {
      d0 = memory.vram[pos +  0];
      d1 = memory.vram[pos +  1];
      d2 = memory.vram[pos + 16];
      d3 = memory.vram[pos + 17];
      d4 = memory.vram[pos + 32];
      d5 = memory.vram[pos + 33];
      d6 = memory.vram[pos + 48];
      d7 = memory.vram[pos + 49];
      render_bg_tile_line_8bpp(0x80);
      render_bg_tile_line_8bpp(0x40);
      render_bg_tile_line_8bpp(0x20);
      render_bg_tile_line_8bpp(0x10);
      render_bg_tile_line_8bpp(0x08);
      render_bg_tile_line_8bpp(0x04);
      render_bg_tile_line_8bpp(0x02);
      render_bg_tile_line_8bpp(0x01);
      pos += 2;
    }
    bg_tiledataState[Background::Mode::BPP8][tile_num] = 0;
  }

  #undef render_bg_tile_line_2bpp
  #undef render_bg_tile_line_4bpp
  #undef render_bg_tile_line_8bpp
}

auto PPU::flushPixelCache() -> void {
  uint16 above = getPalette(0);
  uint16 below = (r.pseudoHires || r.bgMode == 5 || r.bgMode == 6) ? above : r.color_rgb;

  uint i = 255;
  do {
    pixelCache[i].src_above = above;
    pixelCache[i].src_below = below;
    pixelCache[i].bg_above  = Screen::ID::BACK;
    pixelCache[i].bg_below  = Screen::ID::BACK;
    pixelCache[i].ce_above  = false;
    pixelCache[i].ce_below  = false;
    pixelCache[i].pri_above = 0;
    pixelCache[i].pri_below = 0;
  } while(i--);
}

auto PPU::alloc_tiledata_cache() -> void {
  bg_tiledata[Background::Mode::BPP2]      = new uint8[262144];
  bg_tiledata[Background::Mode::BPP4]      = new uint8[131072];
  bg_tiledata[Background::Mode::BPP8]      = new uint8[ 65536];
  bg_tiledataState[Background::Mode::BPP2] = new uint8[  4096];
  bg_tiledataState[Background::Mode::BPP4] = new uint8[  2048];
  bg_tiledataState[Background::Mode::BPP8] = new uint8[  1024];
}

//marks all tiledata cache entries as dirty
auto PPU::flushTiledataCache() -> void {
  for(uint i : range(4096)) bg_tiledataState[Background::Mode::BPP2][i] = 1;
  for(uint i : range(2048)) bg_tiledataState[Background::Mode::BPP4][i] = 1;
  for(uint i : range(1024)) bg_tiledataState[Background::Mode::BPP8][i] = 1;
}

auto PPU::freeTiledataCache() -> void {
  delete[] bg_tiledata[Background::Mode::BPP2];
  delete[] bg_tiledata[Background::Mode::BPP4];
  delete[] bg_tiledata[Background::Mode::BPP8];
  delete[] bg_tiledataState[Background::Mode::BPP2];
  delete[] bg_tiledataState[Background::Mode::BPP4];
  delete[] bg_tiledataState[Background::Mode::BPP8];
}
