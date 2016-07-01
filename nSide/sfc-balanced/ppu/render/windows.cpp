auto PPU::buildWindowTable(uint bg_id, bool screen) -> void {
  bool set = 1, clr = 0;
  uint8* table = screen == Background::Screen::Above ? windowCache[bg_id].above : windowCache[bg_id].below;

  if(bg_id != Window::ID::COL) {
    bool window_aboveEnable;
    bool window_belowEnable;
    switch(bg_id) {
    case Background::ID::BG1:
      window_aboveEnable = window.io.bg1.aboveEnable;
      window_belowEnable = window.io.bg1.belowEnable;
      break;
    case Background::ID::BG2:
      window_aboveEnable = window.io.bg2.aboveEnable;
      window_belowEnable = window.io.bg2.belowEnable;
      break;
    case Background::ID::BG3:
      window_aboveEnable = window.io.bg3.aboveEnable;
      window_belowEnable = window.io.bg3.belowEnable;
      break;
    case Background::ID::BG4:
      window_aboveEnable = window.io.bg4.aboveEnable;
      window_belowEnable = window.io.bg4.belowEnable;
      break;
    case Object::ID::OBJ:
      window_aboveEnable = window.io.obj.aboveEnable;
      window_belowEnable = window.io.obj.belowEnable;
      break;
    }
    if(screen == Background::Screen::Above && !window_aboveEnable) {
      memset(table, 0, 256);
      return;
    }
    if(screen == Background::Screen::Below && !window_belowEnable) {
      memset(table, 0, 256);
      return;
    }
  } else {
    switch(screen == Background::Screen::Above ? window.io.col.aboveMask : window.io.col.belowMask) {
    case 0: memset(table, 1, 256); return;  //always
    case 3: memset(table, 0, 256); return;  //never
    case 1: set = 1, clr = 0; break;        //inside window only
    case 2: set = 0, clr = 1; break;        //outside window only
    }
  }

  bool window_oneEnable;
  bool window_oneInvert;
  bool window_twoEnable;
  bool window_twoInvert;
  uint2 window_mask;
  switch(bg_id) {
  case Background::ID::BG1:
    window_oneEnable = window.io.bg1.oneEnable;
    window_oneInvert = window.io.bg1.oneInvert;
    window_twoEnable = window.io.bg1.twoEnable;
    window_twoInvert = window.io.bg1.twoInvert;
    window_mask = window.io.bg1.mask;
    break;
  case Background::ID::BG2:
    window_oneEnable = window.io.bg2.oneEnable;
    window_oneInvert = window.io.bg2.oneInvert;
    window_twoEnable = window.io.bg2.twoEnable;
    window_twoInvert = window.io.bg2.twoInvert;
    window_mask = window.io.bg2.mask;
    break;
  case Background::ID::BG3:
    window_oneEnable = window.io.bg3.oneEnable;
    window_oneInvert = window.io.bg3.oneInvert;
    window_twoEnable = window.io.bg3.twoEnable;
    window_twoInvert = window.io.bg3.twoInvert;
    window_mask = window.io.bg3.mask;
    break;
  case Background::ID::BG4:
    window_oneEnable = window.io.bg4.oneEnable;
    window_oneInvert = window.io.bg4.oneInvert;
    window_twoEnable = window.io.bg4.twoEnable;
    window_twoInvert = window.io.bg4.twoInvert;
    window_mask = window.io.bg4.mask;
    break;
  case Object::ID::OBJ:
    window_oneEnable = window.io.obj.oneEnable;
    window_oneInvert = window.io.obj.oneInvert;
    window_twoEnable = window.io.obj.twoEnable;
    window_twoInvert = window.io.obj.twoInvert;
    window_mask = window.io.obj.mask;
    break;
  case Window::ID::COL:
    window_oneEnable = window.io.col.oneEnable;
    window_oneInvert = window.io.col.oneInvert;
    window_twoEnable = window.io.col.twoEnable;
    window_twoInvert = window.io.col.twoInvert;
    window_mask = window.io.col.mask;
    break;
  }
  if(!window_oneEnable && !window_twoEnable) {
    memset(table, clr, 256);
    return;
  }

  if( window_oneEnable && !window_twoEnable) {
    if(window_oneInvert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= window.io.oneLeft && x <= window.io.oneRight) ? set : clr;
    }
    return;
  }

  if(!window_oneEnable &&  window_twoEnable) {
    if(window_twoInvert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= window.io.twoLeft && x <= window.io.twoRight) ? set : clr;
    }
    return;
  }

  for(uint x : range(256)) {
    bool w1_mask = (x >= window.io.oneLeft && x <= window.io.oneRight) ^ window_oneInvert;
    bool w2_mask = (x >= window.io.twoLeft && x <= window.io.twoRight) ^ window_twoInvert;

    switch(window_mask) {
    case 0: table[x] = (w1_mask | w2_mask) == 1 ? set : clr; break;  //or
    case 1: table[x] = (w1_mask & w2_mask) == 1 ? set : clr; break;  //and
    case 2: table[x] = (w1_mask ^ w2_mask) == 1 ? set : clr; break;  //xor
    case 3: table[x] = (w1_mask ^ w2_mask) == 0 ? set : clr; break;  //xnor
    }
  }
}

auto PPU::buildWindowTables(uint bg_id) -> void {
  buildWindowTable(bg_id, Background::Screen::Above);
  buildWindowTable(bg_id, Background::Screen::Below);
}
