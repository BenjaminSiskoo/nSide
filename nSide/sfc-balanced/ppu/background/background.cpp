auto PPU::Background::getTile(uint x, uint y) -> uint16 {
  x = (x & ppu.bg_info[id].maskX) >> ppu.bg_info[id].tileWidth;
  y = (y & ppu.bg_info[id].maskY) >> ppu.bg_info[id].tileHeight;

  uint16 offset = ((y & 0x1f) << 5) + (x & 0x1f);
  if(x & 0x20) offset += ppu.bg_info[id].screenX;
  if(y & 0x20) offset += ppu.bg_info[id].screenY;

  const uint16 address = io.screenAddress + offset;
  return ppu.vram[address];
}

auto PPU::Background::reset() -> void {
  io.tiledataAddress = 0x0000;
  io.screenAddress = 0x0000;
  io.screenSize = ScreenSize::Size32x32;
  io.mosaicEnabled = false;
  io.tileSize = TileSize::Size8x8;
  io.mode = 0;
  for(auto& p : io.priority) p = 0;
  io.aboveEnable = false;
  io.belowEnable = false;
  io.hoffset = 0x0000;
  io.voffset = 0x0000;

  //x = 0;
  y = 0;

  tile = 0;
  priority = 0;
  paletteNumber = 0;
  paletteIndex = 0;
}
