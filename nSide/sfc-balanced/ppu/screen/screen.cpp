//color addition / subtraction
//thanks go to blargg for the optimized algorithms
auto PPU::Screen::blend(uint x, uint y) const -> uint15 {
  if(!io.colorMode) {
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

auto PPU::Screen::paletteColor(uint8 palette) const -> uint15 {
  ppu.latch.cgramAddress = palette;
  return cgram[palette];
}

auto PPU::Screen::directColor(uint8 palette, uint16 tile) const -> uint15 {
  //palette = -------- BBGGGRRR
  //tile    = ---bgr-- --------
  //output  = 0BBb00GG Gg0RRRr0
  return ((palette << 7) & 0x6000) | ((tile << 10) & 0x1000)
       | ((palette << 4) & 0x0380) | ((tile <<  5) & 0x0040)
       | ((palette << 2) & 0x001c) | ((tile <<  1) & 0x0002);
}

auto PPU::Screen::reset() -> void {
  io.blendMode = false;
  io.directColor = false;
  io.colorMode = false;
  io.colorHalve = false;
  io.bg1.colorEnable = false;
  io.bg2.colorEnable = false;
  io.bg3.colorEnable = false;
  io.bg4.colorEnable = false;
  io.obj.colorEnable = false;
  io.back.colorEnable = false;
  io.colorBlue = 0;
  io.colorGreen = 0;
  io.colorRed = 0;
}
