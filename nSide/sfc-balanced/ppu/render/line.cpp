inline auto PPU::get_palette(uint8 index) -> uint16 {
  const uint addr = index << 1;
  return cgram[addr] + (cgram[addr + 1] << 8);
}

//p = 00000bgr <palette data>
//t = BBGGGRRR <tilemap data>
//r = 0BBb00GGGg0RRRr0 <return data>
inline auto PPU::get_direct_color(uint8 p, uint8 t) -> uint16 {
  return ((t & 7) << 2) | ((p & 1) << 1) |
    (((t >> 3) & 7) << 7) | (((p >> 1) & 1) << 6) |
    ((t >> 6) << 13) | ((p >> 2) << 12);
}

inline auto PPU::get_pixel_normal(uint32 x) -> uint16 {
  pixel_t& p = pixelCache[x];
  uint16 src_main, src_sub;
  uint8 bg_sub;

  src_main = p.src_main;

  if(!screen.regs.addsub_mode) {
    bg_sub  = BACK;
    src_sub = regs.color_rgb;
  } else {
    bg_sub  = p.bg_sub;
    src_sub = p.src_sub;
  }

  if(!windowCache[COL].main[x]) {
    if(!windowCache[COL].sub[x]) {
      return 0x0000;
    }
    src_main = 0x0000;
  }

  bool color_enable;
  switch(p.bg_main) {
  case Background::ID::BG1: color_enable = screen.regs.bg1_color_enable; break;
  case Background::ID::BG2: color_enable = screen.regs.bg2_color_enable; break;
  case Background::ID::BG3: color_enable = screen.regs.bg3_color_enable; break;
  case Background::ID::BG4: color_enable = screen.regs.bg4_color_enable; break;
  case OAM:                 color_enable = screen.regs.oam_color_enable; break;
  case BACK:                color_enable = screen.regs.back_color_enable; break;
  }

  if(!p.ce_main && color_enable && windowCache[COL].sub[x]) {
    bool halve = false;
    if(screen.regs.color_halve && windowCache[COL].main[x]) {
      if(screen.regs.addsub_mode && bg_sub == BACK);
      else {
        halve = true;
      }
    }
    return addsub(src_main, src_sub, halve);
  }

  return src_main;
}

inline auto PPU::get_pixel_swap(uint32 x) -> uint16 {
  pixel_t& p = pixelCache[x];
  uint16 src_main, src_sub;
  uint8 bg_sub;

  src_main = p.src_sub;

  if(!screen.regs.addsub_mode) {
    bg_sub  = BACK;
    src_sub = regs.color_rgb;
  } else {
    bg_sub  = p.bg_main;
    src_sub = p.src_main;
  }

  if(!windowCache[COL].main[x]) {
    if(!windowCache[COL].sub[x]) {
      return 0x0000;
    }
    src_main = 0x0000;
  }

  bool color_enable;
  switch(p.bg_sub) {
  case Background::ID::BG1: color_enable = screen.regs.bg1_color_enable; break;
  case Background::ID::BG2: color_enable = screen.regs.bg2_color_enable; break;
  case Background::ID::BG3: color_enable = screen.regs.bg3_color_enable; break;
  case Background::ID::BG4: color_enable = screen.regs.bg4_color_enable; break;
  case OAM:                 color_enable = screen.regs.oam_color_enable; break;
  case BACK:                color_enable = screen.regs.back_color_enable; break;
  }

  if(!p.ce_sub && color_enable && windowCache[COL].sub[x]) {
    bool halve = false;
    if(screen.regs.color_halve && windowCache[COL].main[x]) {
      if(screen.regs.addsub_mode && bg_sub == BACK);
      else {
        halve = true;
      }
    }
    return addsub(src_main, src_sub, halve);
  }

  return src_main;
}

inline auto PPU::render_line_output() -> void {
  auto lineA = (uint32*)output + line * 1024;
  auto lineB = lineA + (interlace() ? 0 : 512);
  if(interlace() && field()) lineA += 512, lineB += 512;

  if(!regs.pseudo_hires && regs.bgmode != 5 && regs.bgmode != 6) {
    for(uint x : range(256)) {
      uint color = (regs.display_brightness << 15) | get_pixel_normal(x);
      *lineA++ = *lineB++ = color;
      *lineA++ = *lineB++ = color;
    }
  } else {
    for(uint x : range(256)) {
      *lineA++ = *lineB++ = (regs.display_brightness << 15) | get_pixel_swap(x);
      *lineA++ = *lineB++ = (regs.display_brightness << 15) | get_pixel_normal(x);
    }
  }
}

inline auto PPU::render_line_clear() -> void {
  auto lineA = (uint32*)output + line * 1024;
  auto lineB = lineA + (interlace() ? 0 : 512);
  if(interlace() && field()) lineA += 512, lineB += 512;
  memory::fill(lineA, 512 * sizeof(uint32));
  memory::fill(lineB, 512 * sizeof(uint32));
}
