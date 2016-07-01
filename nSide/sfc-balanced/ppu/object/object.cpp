#include "oam.cpp"

auto PPU::Object::addressReset() -> void {
  ppu.io.oamAddress = ppu.io.oamBaseAddress;
  setFirstSprite();
}

auto PPU::Object::setFirstSprite() -> void {
  io.firstSprite = !ppu.io.oamPriority ? 0 : (ppu.io.oamAddress >> 2) & 127;
}

auto PPU::Object::reset() -> void {
  for(auto& object : oam.object) {
    object.x = 0;
    object.y = 0;
    object.character = 0;
    object.nameselect = 0;
    object.vflip = 0;
    object.hflip = 0;
    object.priority = 0;
    object.palette = 0;
    object.size = 0;
  }

  t.itemCount = 0;
  t.tileCount = 0;

  io.aboveEnable = false;
  io.belowEnable = false;
  io.interlace = false;

  io.baseSize = 0;
  io.nameselect = 0;
  io.tiledataAddress = 0x0000;
  io.firstSprite = 0;

  for(auto& p : io.priority) p = 0;

  io.timeOver = false;
  io.rangeOver = false;
}
