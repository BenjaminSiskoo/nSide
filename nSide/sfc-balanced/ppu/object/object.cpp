#include "list.cpp"

auto PPU::Object::addressReset() -> void {
  ppu.r.oamAddress = ppu.r.oamBaseAddress;
  setFirstSprite();
}

auto PPU::Object::setFirstSprite() -> void {
  r.firstSprite = !ppu.r.oamPriority ? 0 : (ppu.r.oamAddress >> 2) & 127;
}

auto PPU::Object::reset() -> void {
  t.itemCount = 0;
  t.tileCount = 0;

  r.aboveEnable = false;
  r.belowEnable = false;
  r.interlace = false;

  r.baseSize = 0;
  r.nameSelect = 0;
  r.tiledataAddress = 0x0000;
  r.firstSprite = 0;

  for(auto& p : r.priority) p = 0;

  r.timeOver = false;
  r.rangeOver = false;
}
