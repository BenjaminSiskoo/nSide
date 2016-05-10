template<uint color_depth>
auto PPU::render_bg_tile(uint16 tile_num) -> void {
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

  if(color_depth == BPP2) {
    uint8* dest = (uint8*)bg_tiledata[BPP2] + tile_num * 64;
    uint pos = tile_num * 16;
    uint y = 8;
    while(y--) {
      d0 = vram[pos    ];
      d1 = vram[pos + 1];
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
    bg_tiledata_state[BPP2][tile_num] = 0;
  }

  if(color_depth == BPP4) {
    uint8* dest = (uint8*)bg_tiledata[BPP4] + tile_num * 64;
    uint pos = tile_num * 32;
    uint y = 8;
    while(y--) {
      d0 = vram[pos     ];
      d1 = vram[pos +  1];
      d2 = vram[pos + 16];
      d3 = vram[pos + 17];
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
    bg_tiledata_state[BPP4][tile_num] = 0;
  }

  if(color_depth == BPP8) {
    uint8* dest = (uint8*)bg_tiledata[BPP8] + tile_num * 64;
    uint pos = tile_num * 64;
    uint y = 8;
    while(y--) {
      d0 = vram[pos     ];
      d1 = vram[pos +  1];
      d2 = vram[pos + 16];
      d3 = vram[pos + 17];
      d4 = vram[pos + 32];
      d5 = vram[pos + 33];
      d6 = vram[pos + 48];
      d7 = vram[pos + 49];
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
    bg_tiledata_state[BPP8][tile_num] = 0;
  }

  #undef render_bg_tile_line_2bpp
  #undef render_bg_tile_line_4bpp
  #undef render_bg_tile_line_8bpp
}

auto PPU::flush_pixel_cache() -> void {
  uint16 main = get_palette(0);
  uint16 sub  = (regs.pseudo_hires || regs.bg_mode == 5 || regs.bg_mode == 6)
              ? main
              : regs.color_rgb;

  uint i = 255;
  do {
    pixel_cache[i].src_main = main;
    pixel_cache[i].src_sub  = sub;
    pixel_cache[i].bg_main  = BACK;
    pixel_cache[i].bg_sub   = BACK;
    pixel_cache[i].ce_main  = false;
    pixel_cache[i].ce_sub   = false;
    pixel_cache[i].pri_main = 0;
    pixel_cache[i].pri_sub  = 0;
  } while(i--);
}

auto PPU::alloc_tiledata_cache() -> void {
  bg_tiledata[BPP2]       = new uint8[262144]();
  bg_tiledata[BPP4]       = new uint8[131072]();
  bg_tiledata[BPP8]       = new uint8[ 65536]();
  bg_tiledata_state[BPP2] = new uint8[  4096]();
  bg_tiledata_state[BPP4] = new uint8[  2048]();
  bg_tiledata_state[BPP8] = new uint8[  1024]();
}

//marks all tiledata cache entries as dirty
auto PPU::flush_tiledata_cache() -> void {
  for(uint i : range(4096)) bg_tiledata_state[BPP2][i] = 1;
  for(uint i : range(2048)) bg_tiledata_state[BPP4][i] = 1;
  for(uint i : range(1024)) bg_tiledata_state[BPP8][i] = 1;
}

auto PPU::free_tiledata_cache() -> void {
  delete[] bg_tiledata[BPP2];
  delete[] bg_tiledata[BPP4];
  delete[] bg_tiledata[BPP8];
  delete[] bg_tiledata_state[BPP2];
  delete[] bg_tiledata_state[BPP4];
  delete[] bg_tiledata_state[BPP8];
}
