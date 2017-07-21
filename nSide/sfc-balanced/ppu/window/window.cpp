auto PPU::Window::power() -> void {
  io.bg1.oneEnable = false;
  io.bg1.oneInvert = false;
  io.bg1.twoEnable = false;
  io.bg1.twoInvert = false;
  io.bg1.mask = 0;
  io.bg1.aboveEnable = false;
  io.bg1.belowEnable = false;

  io.bg2.oneEnable = false;
  io.bg2.oneInvert = false;
  io.bg2.twoEnable = false;
  io.bg2.twoInvert = false;
  io.bg2.mask = 0;
  io.bg2.aboveEnable = false;
  io.bg2.belowEnable = false;

  io.bg3.oneEnable = false;
  io.bg3.oneInvert = false;
  io.bg3.twoEnable = false;
  io.bg3.twoInvert = false;
  io.bg3.mask = 0;
  io.bg3.aboveEnable = false;
  io.bg3.belowEnable = false;

  io.bg4.oneEnable = false;
  io.bg4.oneInvert = false;
  io.bg4.twoEnable = false;
  io.bg4.twoInvert = false;
  io.bg4.mask = 0;
  io.bg4.aboveEnable = false;
  io.bg4.belowEnable = false;

  io.obj.oneEnable = false;
  io.obj.oneInvert = false;
  io.obj.twoEnable = false;
  io.obj.twoInvert = false;
  io.obj.mask = 0;
  io.obj.aboveEnable = false;
  io.obj.belowEnable = false;

  io.col.oneEnable = false;
  io.col.oneInvert = false;
  io.col.twoEnable = false;
  io.col.twoInvert = false;
  io.col.mask = 0;
  io.col.aboveMask = 0;
  io.col.belowMask = 0;

  io.oneLeft = 0x00;
  io.oneRight = 0x00;
  io.twoLeft = 0x00;
  io.twoRight = 0x00;
}

auto PPU::Window::buildTable(uint bg_id, bool screen) -> void {
  bool set = 1, clr = 0;
  uint8* table = screen == Background::Screen::Above ? cache[bg_id].above : cache[bg_id].below;

  if(bg_id != ID::COL) {
    bool aboveEnable;
    bool belowEnable;
    switch(bg_id) {
    case Background::ID::BG1:
      aboveEnable = io.bg1.aboveEnable;
      belowEnable = io.bg1.belowEnable;
      break;
    case Background::ID::BG2:
      aboveEnable = io.bg2.aboveEnable;
      belowEnable = io.bg2.belowEnable;
      break;
    case Background::ID::BG3:
      aboveEnable = io.bg3.aboveEnable;
      belowEnable = io.bg3.belowEnable;
      break;
    case Background::ID::BG4:
      aboveEnable = io.bg4.aboveEnable;
      belowEnable = io.bg4.belowEnable;
      break;
    case Object::ID::OBJ:
      aboveEnable = io.obj.aboveEnable;
      belowEnable = io.obj.belowEnable;
      break;
    }
    if(screen == Background::Screen::Above && !aboveEnable) {
      memset(table, 0, 256);
      return;
    }
    if(screen == Background::Screen::Below && !belowEnable) {
      memset(table, 0, 256);
      return;
    }
  } else {
    switch(screen == Background::Screen::Above ? io.col.aboveMask : io.col.belowMask) {
    case 0: memset(table, 1, 256); return;  //always
    case 3: memset(table, 0, 256); return;  //never
    case 1: set = 1, clr = 0; break;        //inside window only
    case 2: set = 0, clr = 1; break;        //outside window only
    }
  }

  bool oneEnable;
  bool oneInvert;
  bool twoEnable;
  bool twoInvert;
  uint2 mask;
  switch(bg_id) {
  case Background::ID::BG1:
    oneEnable = io.bg1.oneEnable;
    oneInvert = io.bg1.oneInvert;
    twoEnable = io.bg1.twoEnable;
    twoInvert = io.bg1.twoInvert;
    mask = io.bg1.mask;
    break;
  case Background::ID::BG2:
    oneEnable = io.bg2.oneEnable;
    oneInvert = io.bg2.oneInvert;
    twoEnable = io.bg2.twoEnable;
    twoInvert = io.bg2.twoInvert;
    mask = io.bg2.mask;
    break;
  case Background::ID::BG3:
    oneEnable = io.bg3.oneEnable;
    oneInvert = io.bg3.oneInvert;
    twoEnable = io.bg3.twoEnable;
    twoInvert = io.bg3.twoInvert;
    mask = io.bg3.mask;
    break;
  case Background::ID::BG4:
    oneEnable = io.bg4.oneEnable;
    oneInvert = io.bg4.oneInvert;
    twoEnable = io.bg4.twoEnable;
    twoInvert = io.bg4.twoInvert;
    mask = io.bg4.mask;
    break;
  case Object::ID::OBJ:
    oneEnable = io.obj.oneEnable;
    oneInvert = io.obj.oneInvert;
    twoEnable = io.obj.twoEnable;
    twoInvert = io.obj.twoInvert;
    mask = io.obj.mask;
    break;
  case Window::ID::COL:
    oneEnable = io.col.oneEnable;
    oneInvert = io.col.oneInvert;
    twoEnable = io.col.twoEnable;
    twoInvert = io.col.twoInvert;
    mask = io.col.mask;
    break;
  }
  if(!oneEnable && !twoEnable) {
    memset(table, clr, 256);
    return;
  }

  if( oneEnable && !twoEnable) {
    if(oneInvert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= io.oneLeft && x <= io.oneRight) ? set : clr;
    }
    return;
  }

  if(!oneEnable &&  twoEnable) {
    if(twoInvert) set ^= clr ^= set ^= clr;
    for(uint x : range(256)) {
      table[x] = (x >= io.twoLeft && x <= io.twoRight) ? set : clr;
    }
    return;
  }

  for(uint x : range(256)) {
    bool w1_mask = (x >= io.oneLeft && x <= io.oneRight) ^ oneInvert;
    bool w2_mask = (x >= io.twoLeft && x <= io.twoRight) ^ twoInvert;

    switch(mask) {
    case 0: table[x] = (w1_mask | w2_mask) == 1 ? set : clr; break;  //or
    case 1: table[x] = (w1_mask & w2_mask) == 1 ? set : clr; break;  //and
    case 2: table[x] = (w1_mask ^ w2_mask) == 1 ? set : clr; break;  //xor
    case 3: table[x] = (w1_mask ^ w2_mask) == 0 ? set : clr; break;  //xnor
    }
  }
}

auto PPU::Window::buildTables(uint bg_id) -> void {
  buildTable(bg_id, Background::Screen::Above);
  buildTable(bg_id, Background::Screen::Below);
}
