auto PPU::Background::getTile(uint x, uint y) -> uint {
  x = (x & ppu.bg_info[id].mx) >> ppu.bg_info[id].tw;
  y = (y & ppu.bg_info[id].my) >> ppu.bg_info[id].th;

  uint16 pos = ((y & 0x1f) << 5) + (x & 0x1f);
  if(y & 0x20) pos += ppu.bg_info[id].scy;
  if(x & 0x20) pos += ppu.bg_info[id].scx;

  const uint16 addr = r.screenAddress + (pos << 1);
  return ppu.memory.vram[addr] + (ppu.memory.vram[addr + 1] << 8);
}

auto PPU::Background::reset() -> void {
  r.tiledataAddress = 0x0000;
  r.screenAddress = 0x0000;
  r.screenSize = ScreenSize::Size32x32;
  r.mosaicEnabled = false;
  r.tileSize = TileSize::Size8x8;
  r.mode = 0;
  for(auto& p : r.priority) p = 0;
  r.aboveEnable = false;
  r.belowEnable = false;
  r.hoffset = 0x0000;
  r.voffset = 0x0000;
}
