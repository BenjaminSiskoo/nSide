auto PPU::build_window_table(uint8 bg_id, bool screen) -> void {
  bool set = 1, clr = 0;
  uint8* table = screen == Background::Screen::Main ? windowCache[bg_id].main : windowCache[bg_id].sub;

  if(bg_id != COL) {
    bool main_window_enable;
    bool sub_window_enable;
    switch(bg_id) {
    case Background::ID::BG1:
      main_window_enable = window.regs.bg1_main_enable;
      sub_window_enable  = window.regs.bg1_sub_enable;
      break;
    case Background::ID::BG2:
      main_window_enable = window.regs.bg2_main_enable;
      sub_window_enable  = window.regs.bg2_sub_enable;
      break;
    case Background::ID::BG3:
      main_window_enable = window.regs.bg3_main_enable;
      sub_window_enable  = window.regs.bg3_sub_enable;
      break;
    case Background::ID::BG4:
      main_window_enable = window.regs.bg4_main_enable;
      sub_window_enable  = window.regs.bg4_sub_enable;
      break;
    case OAM:
      main_window_enable = window.regs.oam_main_enable;
      sub_window_enable  = window.regs.oam_sub_enable;
      break;
    }
    if(screen == Background::Screen::Main && !main_window_enable) {
      memset(table, 0, 256);
      return;
    }
    if(screen == Background::Screen::Sub && !sub_window_enable) {
      memset(table, 0, 256);
      return;
    }
  } else {
    switch(screen == Background::Screen::Main ? window.regs.col_main_mask : window.regs.col_sub_mask) {
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
    window_one_enable = window.regs.bg1_one_enable;
    window_one_invert = window.regs.bg1_one_invert;
    window_two_enable = window.regs.bg1_two_enable;
    window_two_invert = window.regs.bg1_two_invert;
    window_mask = window.regs.bg1_mask;
    break;
  case Background::ID::BG2:
    window_one_enable = window.regs.bg2_one_enable;
    window_one_invert = window.regs.bg2_one_invert;
    window_two_enable = window.regs.bg2_two_enable;
    window_two_invert = window.regs.bg2_two_invert;
    window_mask = window.regs.bg2_mask;
    break;
  case Background::ID::BG3:
    window_one_enable = window.regs.bg3_one_enable;
    window_one_invert = window.regs.bg3_one_invert;
    window_two_enable = window.regs.bg3_two_enable;
    window_two_invert = window.regs.bg3_two_invert;
    window_mask = window.regs.bg3_mask;
    break;
  case Background::ID::BG4:
    window_one_enable = window.regs.bg4_one_enable;
    window_one_invert = window.regs.bg4_one_invert;
    window_two_enable = window.regs.bg4_two_enable;
    window_two_invert = window.regs.bg4_two_invert;
    window_mask = window.regs.bg4_mask;
    break;
  case OAM:
    window_one_enable = window.regs.oam_one_enable;
    window_one_invert = window.regs.oam_one_invert;
    window_two_enable = window.regs.oam_two_enable;
    window_two_invert = window.regs.oam_two_invert;
    window_mask = window.regs.oam_mask;
    break;
  case COL:
    window_one_enable = window.regs.col_one_enable;
    window_one_invert = window.regs.col_one_invert;
    window_two_enable = window.regs.col_two_enable;
    window_two_invert = window.regs.col_two_invert;
    window_mask = window.regs.col_mask;
    break;
  }
  if(!window_one_enable && !window_two_enable) {
    memset(table, clr, 256);
    return;
  }

  if( window_one_enable && !window_two_enable) {
    if(window_one_invert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= window.regs.one_left && x <= window.regs.one_right) ? set : clr;
    }
    return;
  }

  if(!window_one_enable &&  window_two_enable) {
    if(window_two_invert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= window.regs.two_left && x <= window.regs.two_right) ? set : clr;
    }
    return;
  }

  for(uint x : range(256)) {
    bool w1_mask = (x >= window.regs.one_left && x <= window.regs.one_right) ^ window_one_invert;
    bool w2_mask = (x >= window.regs.two_left && x <= window.regs.two_right) ^ window_two_invert;

    switch(window_mask) {
    case 0: table[x] = (w1_mask | w2_mask) == 1 ? set : clr; break;  //or
    case 1: table[x] = (w1_mask & w2_mask) == 1 ? set : clr; break;  //and
    case 2: table[x] = (w1_mask ^ w2_mask) == 1 ? set : clr; break;  //xor
    case 3: table[x] = (w1_mask ^ w2_mask) == 0 ? set : clr; break;  //xnor
    }
  }
}

auto PPU::build_window_tables(uint8 bg_id) -> void {
  build_window_table(bg_id, Background::Screen::Main);
  build_window_table(bg_id, Background::Screen::Sub);
}
