auto PPU::buildWindowTable(uint bg_id, bool screen) -> void {
  bool set = 1, clr = 0;
  uint8* table = screen == Background::Screen::Above ? windowCache[bg_id].above : windowCache[bg_id].below;

  if(bg_id != Window::ID::COL) {
    bool above_window_enable;
    bool below_window_enable;
    switch(bg_id) {
    case Background::ID::BG1:
      above_window_enable = window.r.bg1.aboveEnable;
      below_window_enable = window.r.bg1.belowEnable;
      break;
    case Background::ID::BG2:
      above_window_enable = window.r.bg2.aboveEnable;
      below_window_enable = window.r.bg2.belowEnable;
      break;
    case Background::ID::BG3:
      above_window_enable = window.r.bg3.aboveEnable;
      below_window_enable = window.r.bg3.belowEnable;
      break;
    case Background::ID::BG4:
      above_window_enable = window.r.bg4.aboveEnable;
      below_window_enable = window.r.bg4.belowEnable;
      break;
    case OAM::ID::OAM:
      above_window_enable = window.r.oam.aboveEnable;
      below_window_enable = window.r.oam.belowEnable;
      break;
    }
    if(screen == Background::Screen::Above && !above_window_enable) {
      memset(table, 0, 256);
      return;
    }
    if(screen == Background::Screen::Below && !below_window_enable) {
      memset(table, 0, 256);
      return;
    }
  } else {
    switch(screen == Background::Screen::Above ? window.r.col.aboveMask : window.r.col.belowMask) {
    case 0: memset(table, 1, 256); return;  //always
    case 3: memset(table, 0, 256); return;  //never
    case 1: set = 1, clr = 0; break;        //inside window only
    case 2: set = 0, clr = 1; break;        //outside window only
    }
  }

  bool window_one_enable;
  bool window_one_invert;
  bool window_two_enable;
  bool window_two_invert;
  uint2 window_mask;
  switch(bg_id) {
  case Background::ID::BG1:
    window_one_enable = window.r.bg1.oneEnable;
    window_one_invert = window.r.bg1.oneInvert;
    window_two_enable = window.r.bg1.twoEnable;
    window_two_invert = window.r.bg1.twoInvert;
    window_mask = window.r.bg1.mask;
    break;
  case Background::ID::BG2:
    window_one_enable = window.r.bg2.oneEnable;
    window_one_invert = window.r.bg2.oneInvert;
    window_two_enable = window.r.bg2.twoEnable;
    window_two_invert = window.r.bg2.twoInvert;
    window_mask = window.r.bg2.mask;
    break;
  case Background::ID::BG3:
    window_one_enable = window.r.bg3.oneEnable;
    window_one_invert = window.r.bg3.oneInvert;
    window_two_enable = window.r.bg3.twoEnable;
    window_two_invert = window.r.bg3.twoInvert;
    window_mask = window.r.bg3.mask;
    break;
  case Background::ID::BG4:
    window_one_enable = window.r.bg4.oneEnable;
    window_one_invert = window.r.bg4.oneInvert;
    window_two_enable = window.r.bg4.twoEnable;
    window_two_invert = window.r.bg4.twoInvert;
    window_mask = window.r.bg4.mask;
    break;
  case OAM::ID::OAM:
    window_one_enable = window.r.oam.oneEnable;
    window_one_invert = window.r.oam.oneInvert;
    window_two_enable = window.r.oam.twoEnable;
    window_two_invert = window.r.oam.twoInvert;
    window_mask = window.r.oam.mask;
    break;
  case Window::ID::COL:
    window_one_enable = window.r.col.oneEnable;
    window_one_invert = window.r.col.oneInvert;
    window_two_enable = window.r.col.twoEnable;
    window_two_invert = window.r.col.twoInvert;
    window_mask = window.r.col.mask;
    break;
  }
  if(!window_one_enable && !window_two_enable) {
    memset(table, clr, 256);
    return;
  }

  if( window_one_enable && !window_two_enable) {
    if(window_one_invert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= window.r.oneLeft && x <= window.r.oneRight) ? set : clr;
    }
    return;
  }

  if(!window_one_enable &&  window_two_enable) {
    if(window_two_invert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= window.r.twoLeft && x <= window.r.twoRight) ? set : clr;
    }
    return;
  }

  for(uint x : range(256)) {
    bool w1_mask = (x >= window.r.oneLeft && x <= window.r.oneRight) ^ window_one_invert;
    bool w2_mask = (x >= window.r.twoLeft && x <= window.r.twoRight) ^ window_two_invert;

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
