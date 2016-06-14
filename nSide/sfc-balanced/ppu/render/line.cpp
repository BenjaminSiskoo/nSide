inline auto PPU::getPalette(uint8 index) -> uint16 {
  const uint addr = index << 1;
  return memory.cgram[addr] + (memory.cgram[addr + 1] << 8);
}

//p = 00000bgr <palette data>
//t = BBGGGRRR <tilemap data>
//r = 0BBb00GGGg0RRRr0 <return data>
inline auto PPU::getDirectColor(uint8 p, uint8 t) -> uint16 {
  return ((t & 7) << 2) | ((p & 1) << 1) |
    (((t >> 3) & 7) << 7) | (((p >> 1) & 1) << 6) |
    ((t >> 6) << 13) | ((p >> 2) << 12);
}

inline auto PPU::get_pixel_normal(uint32 x) -> uint16 {
  pixel_t& p = pixelCache[x];
  uint16 src_above, src_below;
  uint8 bg_below;

  src_above = p.src_above;

  if(!screen.r.blendMode) {
    bg_below  = Screen::ID::BACK;
    src_below = r.color_rgb;
  } else {
    bg_below  = p.bg_below;
    src_below = p.src_below;
  }

  if(!windowCache[Window::ID::COL].above[x]) {
    if(!windowCache[Window::ID::COL].below[x]) {
      return 0x0000;
    }
    src_above = 0x0000;
  }

  bool color_enable;
  switch(p.bg_above) {
  case Background::ID::BG1: color_enable = screen.r.bg1.colorEnable; break;
  case Background::ID::BG2: color_enable = screen.r.bg2.colorEnable; break;
  case Background::ID::BG3: color_enable = screen.r.bg3.colorEnable; break;
  case Background::ID::BG4: color_enable = screen.r.bg4.colorEnable; break;
  case OAM::ID::OAM:        color_enable = screen.r.oam.colorEnable; break;
  case Screen::ID::BACK:    color_enable = screen.r.back.colorEnable; break;
  }

  if(!p.ce_above && color_enable && windowCache[Window::ID::COL].below[x]) {
    screen.math.colorHalve = false;
    if(screen.r.colorHalve && windowCache[Window::ID::COL].above[x]) {
      if(screen.r.blendMode && bg_below == Screen::ID::BACK);
      else {
        screen.math.colorHalve = true;
      }
    }
    return screen.blend(src_above, src_below);
  }

  return src_above;
}

inline auto PPU::get_pixel_swap(uint32 x) -> uint16 {
  pixel_t& p = pixelCache[x];
  uint16 src_above, src_below;
  uint8 bg_below;

  src_above = p.src_below;

  if(!screen.r.blendMode) {
    bg_below  = Screen::ID::BACK;
    src_below = r.color_rgb;
  } else {
    bg_below  = p.bg_above;
    src_below = p.src_above;
  }

  if(!windowCache[Window::ID::COL].above[x]) {
    if(!windowCache[Window::ID::COL].below[x]) {
      return 0x0000;
    }
    src_above = 0x0000;
  }

  bool color_enable;
  switch(p.bg_below) {
  case Background::ID::BG1: color_enable = screen.r.bg1.colorEnable; break;
  case Background::ID::BG2: color_enable = screen.r.bg2.colorEnable; break;
  case Background::ID::BG3: color_enable = screen.r.bg3.colorEnable; break;
  case Background::ID::BG4: color_enable = screen.r.bg4.colorEnable; break;
  case OAM::ID::OAM:        color_enable = screen.r.oam.colorEnable; break;
  case Screen::ID::BACK:    color_enable = screen.r.back.colorEnable; break;
  }

  if(!p.ce_below && color_enable && windowCache[Window::ID::COL].below[x]) {
    screen.math.colorHalve = false;
    if(screen.r.colorHalve && windowCache[Window::ID::COL].above[x]) {
      if(screen.r.blendMode && bg_below == Screen::ID::BACK);
      else {
        screen.math.colorHalve = true;
      }
    }
    return screen.blend(src_above, src_below);
  }

  return src_above;
}

inline auto PPU::renderLine_output() -> void {
  uint32* lineA = output + line * 1024;
  uint32* lineB = lineA + (interlace() ? 0 : 512);
  if(interlace() && field()) lineA += 512, lineB += 512;

  if(!r.pseudoHires && r.bgMode != 5 && r.bgMode != 6) {
    for(uint x : range(256)) {
      uint color = (r.displayBrightness << 15) | get_pixel_normal(x);
      *lineA++ = *lineB++ = color;
      *lineA++ = *lineB++ = color;
    }
  } else {
    for(uint x : range(256)) {
      *lineA++ = *lineB++ = (r.displayBrightness << 15) | get_pixel_swap(x);
      *lineA++ = *lineB++ = (r.displayBrightness << 15) | get_pixel_normal(x);
    }
  }
}

inline auto PPU::clearLine() -> void {
  uint32* lineA = output + line * 1024;
  uint32* lineB = lineA + (interlace() ? 0 : 512);
  if(interlace() && field()) lineA += 512, lineB += 512;
  memory::fill(lineA, 512 * sizeof(uint32));
  memory::fill(lineB, 512 * sizeof(uint32));
}
