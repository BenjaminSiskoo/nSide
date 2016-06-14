auto PPU::Screen::blend(uint x, uint y) const -> uint16 {
  if(!r.colorMode) {
    if(!math.colorHalve) {
      uint sum = x + y;
      uint carry = (sum - ((x ^ y) & 0x0421)) & 0x8420;
      return (sum - carry) | (carry - (carry >> 5));
    } else {
      return (x + y - ((x ^ y) & 0x0421)) >> 1;
    }
  } else {
    uint diff = x - y + 0x8420;
    uint borrow = (diff - ((x ^ y) & 0x8420)) & 0x8420;
    if(!math.colorHalve) {
      return   (diff - borrow) & (borrow - (borrow >> 5));
    } else {
      return (((diff - borrow) & (borrow - (borrow >> 5))) & 0x7bde) >> 1;
    }
  }
}

auto PPU::Screen::reset() -> void {
  r.blendMode = false;
  r.directColor = false;
  r.colorMode = false;
  r.colorHalve = false;
  r.bg1.colorEnable = false;
  r.bg2.colorEnable = false;
  r.bg3.colorEnable = false;
  r.bg4.colorEnable = false;
  r.oam.colorEnable = false;
  r.back.colorEnable = false;
  r.colorBlue = 0;
  r.colorGreen = 0;
  r.colorRed = 0;
}
