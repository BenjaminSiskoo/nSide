inline auto PPU::getPixelBelow(uint32 x) -> uint16 {
  CachePixel& p = pixelCache[x];
  uint15 aboveColor, belowColor;
  uint8 aboveLayer;

  belowColor = p.belowColor;

  if(!screen.io.blendMode) {
    aboveLayer = Screen::ID::BACK;
    aboveColor = io.color_rgb;
  } else {
    aboveLayer = p.aboveLayer;
    aboveColor = p.aboveColor;
  }

  if(!windowCache[Window::ID::COL].above[x]) {
    if(!windowCache[Window::ID::COL].below[x]) {
      return 0x0000;
    }
    belowColor = 0x0000;
  }

  bool colorEnable;
  switch(p.belowLayer) {
  case Background::ID::BG1: colorEnable = screen.io.bg1.colorEnable; break;
  case Background::ID::BG2: colorEnable = screen.io.bg2.colorEnable; break;
  case Background::ID::BG3: colorEnable = screen.io.bg3.colorEnable; break;
  case Background::ID::BG4: colorEnable = screen.io.bg4.colorEnable; break;
  case Object::ID::OBJ:     colorEnable = screen.io.obj.colorEnable; break;
  case Screen::ID::BACK:    colorEnable = screen.io.back.colorEnable; break;
  }

  if(!p.belowColorExemption && colorEnable && windowCache[Window::ID::COL].below[x]) {
    screen.math.colorHalve = false;
    if(screen.io.colorHalve && windowCache[Window::ID::COL].above[x]) {
      if(screen.io.blendMode == 0 || aboveLayer != Screen::ID::BACK) {
        screen.math.colorHalve = true;
      }
    }
    return screen.blend(belowColor, aboveColor);
  }

  return belowColor;
}

inline auto PPU::getPixelAbove(uint32 x) -> uint16 {
  CachePixel& p = pixelCache[x];
  uint15 aboveColor, belowColor;
  uint8 belowLayer;

  aboveColor = p.aboveColor;

  if(!screen.io.blendMode) {
    belowLayer = Screen::ID::BACK;
    belowColor = io.color_rgb;
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
  case Background::ID::BG1: colorEnable = screen.io.bg1.colorEnable; break;
  case Background::ID::BG2: colorEnable = screen.io.bg2.colorEnable; break;
  case Background::ID::BG3: colorEnable = screen.io.bg3.colorEnable; break;
  case Background::ID::BG4: colorEnable = screen.io.bg4.colorEnable; break;
  case Object::ID::OBJ:     colorEnable = screen.io.obj.colorEnable; break;
  case Screen::ID::BACK:    colorEnable = screen.io.back.colorEnable; break;
  }

  if(!p.aboveColorExemption && colorEnable && windowCache[Window::ID::COL].below[x]) {
    screen.math.colorHalve = false;
    if(screen.io.colorHalve && windowCache[Window::ID::COL].above[x]) {
      if(screen.io.blendMode == 0 || belowLayer != Screen::ID::BACK) {
        screen.math.colorHalve = true;
      }
    }
    return screen.blend(aboveColor, belowColor);
  }

  return aboveColor;
}

inline auto PPU::renderLineOutput() -> void {
  uint32* lineA = output + line * 1024;
  uint32* lineB = lineA + (interlace() ? 0 : 512);
  if(interlace() && field()) lineA += 512, lineB += 512;

  if(!io.pseudoHires && io.bgMode != 5 && io.bgMode != 6) {
    for(uint x : range(256)) {
      uint color = (io.displayBrightness << 15) | getPixelAbove(x);
      *lineA++ = *lineB++ = color;
      *lineA++ = *lineB++ = color;
    }
  } else {
    for(uint x : range(256)) {
      *lineA++ = *lineB++ = (io.displayBrightness << 15) | getPixelBelow(x);
      *lineA++ = *lineB++ = (io.displayBrightness << 15) | getPixelAbove(x);
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
