//p = 00000bgr <palette data>
//t = BBGGGRRR <tilemap data>
//r = 0BBb00GGGg0RRRr0 <return data>
inline auto PPU::getDirectColor(uint8 p, uint8 t) -> uint15 {
  return ((t & 7) << 2) | ((p & 1) << 1) |
    (((t >> 3) & 7) << 7) | (((p >> 1) & 1) << 6) |
    ((t >> 6) << 13) | ((p >> 2) << 12);
}

inline auto PPU::get_pixel_normal(uint32 x) -> uint16 {
  CachePixel& p = pixelCache[x];
  uint15 aboveColor, belowColor;
  uint8 belowLayer;

  aboveColor = p.aboveColor;

  if(!screen.r.blendMode) {
    belowLayer = Screen::ID::BACK;
    belowColor = r.color_rgb;
  } else {
    belowLayer = p.belowLayer;
    belowColor = p.belowColor;
  }

  if(!windowCache[Window::ID::COL].above[x]) {
    if(!windowCache[Window::ID::COL].below[x]) {
      return 0x0000;
    }
    aboveColor = 0x0000;
  }

  bool colorEnable;
  switch(p.aboveLayer) {
  case Background::ID::BG1: colorEnable = screen.r.bg1.colorEnable; break;
  case Background::ID::BG2: colorEnable = screen.r.bg2.colorEnable; break;
  case Background::ID::BG3: colorEnable = screen.r.bg3.colorEnable; break;
  case Background::ID::BG4: colorEnable = screen.r.bg4.colorEnable; break;
  case Object::ID::OBJ:     colorEnable = screen.r.obj.colorEnable; break;
  case Screen::ID::BACK:    colorEnable = screen.r.back.colorEnable; break;
  }

  if(!p.aboveColorExemption && colorEnable && windowCache[Window::ID::COL].below[x]) {
    screen.math.colorHalve = false;
    if(screen.r.colorHalve && windowCache[Window::ID::COL].above[x]) {
      if(screen.r.blendMode == 0 || belowLayer != Screen::ID::BACK) {
        screen.math.colorHalve = true;
      }
    }
    return screen.blend(aboveColor, belowColor);
  }

  return aboveColor;
}

inline auto PPU::get_pixel_swap(uint32 x) -> uint16 {
  CachePixel& p = pixelCache[x];
  uint15 aboveColor, belowColor;
  uint8 belowLayer;

  aboveColor = p.belowColor;

  if(!screen.r.blendMode) {
    belowLayer = Screen::ID::BACK;
    belowColor = r.color_rgb;
  } else {
    belowLayer = p.aboveLayer;
    belowColor = p.aboveColor;
  }

  if(!windowCache[Window::ID::COL].above[x]) {
    if(!windowCache[Window::ID::COL].below[x]) {
      return 0x0000;
    }
    aboveColor = 0x0000;
  }

  bool colorEnable;
  switch(p.belowLayer) {
  case Background::ID::BG1: colorEnable = screen.r.bg1.colorEnable; break;
  case Background::ID::BG2: colorEnable = screen.r.bg2.colorEnable; break;
  case Background::ID::BG3: colorEnable = screen.r.bg3.colorEnable; break;
  case Background::ID::BG4: colorEnable = screen.r.bg4.colorEnable; break;
  case Object::ID::OBJ:     colorEnable = screen.r.obj.colorEnable; break;
  case Screen::ID::BACK:    colorEnable = screen.r.back.colorEnable; break;
  }

  if(!p.belowColorExemption && colorEnable && windowCache[Window::ID::COL].below[x]) {
    screen.math.colorHalve = false;
    if(screen.r.colorHalve && windowCache[Window::ID::COL].above[x]) {
      if(screen.r.blendMode == 0 || belowLayer != Screen::ID::BACK) {
        screen.math.colorHalve = true;
      }
    }
    return screen.blend(aboveColor, belowColor);
  }

  return aboveColor;
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
