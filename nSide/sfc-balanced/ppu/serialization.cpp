auto PPUcounter::serialize(serializer& s) -> void {
  s.integer(status.interlace);
  s.integer(status.field);
  s.integer(status.vcounter);
  s.integer(status.hcounter);

  s.array(history.field);
  s.array(history.vcounter);
  s.array(history.hcounter);
  s.integer(history.index);
}

auto PPU::serialize(serializer& s) -> void {
  Thread::serialize(s);
  PPUcounter::serialize(s);

  s.integer(vram.mask);
  s.array(vram.data, vram.mask + 1);
  s.array(oam.data);
  s.array(cgram.data);

  s.integer(ppu1.version);
  s.integer(ppu1.mdr);

  s.integer(ppu2.version);
  s.integer(ppu2.mdr);

  s.integer(line);

  s.integer(display.interlace);
  s.integer(display.overscan);

  s.integer(cache.obj_baseSize);
  s.integer(cache.obj_nameSelect);
  s.integer(cache.obj_tiledataAddress);

  for(uint n : range(4)) s.integer(r.bg_y[n]);

  s.integer(latch.vram);
  s.integer(latch.oam);
  s.integer(latch.cgram);
  s.integer(latch.bgofs);
  s.integer(latch.mode7);
  s.integer(latch.counters);
  s.integer(latch.hcounter);
  s.integer(latch.vcounter);

  s.integer(latch.oamAddress);
  s.integer(latch.cgramAddress);

  s.integer(r.displayDisable);
  s.integer(r.displayBrightness);

  s.integer(r.oamBaseAddress);
  s.integer(r.oamAddress);
  s.integer(r.oamPriority);

  s.integer(r.bgPriority);
  s.integer(r.bgMode);

  s.integer(r.mosaicSize);
  s.integer(r.mosaicCountdown);

  s.integer(r.hoffsetMode7);
  s.integer(r.voffsetMode7);

  s.integer(r.vramIncrementMode);
  s.integer(r.vramMapping);
  s.integer(r.vramIncrementSize);

  s.integer(r.vramAddress);

  s.integer(r.repeatMode7);
  s.integer(r.vflipMode7);
  s.integer(r.hflipMode7);

  s.integer(r.m7a);
  s.integer(r.m7b);
  s.integer(r.m7c);
  s.integer(r.m7d);
  s.integer(r.m7x);
  s.integer(r.m7y);

  s.integer(r.cgramAddress);
  s.integer(r.cgramAddressLatch);

  s.integer(r.color_rgb);

  s.integer(r.extbg);
  s.integer(r.pseudoHires);
  s.integer(r.overscan);
  s.integer(r.interlace);

  s.integer(r.hcounter);
  s.integer(r.vcounter);

  for(uint n : range(256)) {
    s.integer(pixelCache[n].aboveColor);
    s.integer(pixelCache[n].belowColor);
    s.integer(pixelCache[n].aboveLayer);
    s.integer(pixelCache[n].belowLayer);
    s.integer(pixelCache[n].aboveColorExemption);
    s.integer(pixelCache[n].belowColorExemption);
    s.integer(pixelCache[n].abovePriority);
    s.integer(pixelCache[n].belowPriority);
  }

  //better to just take a small speed hit than store all of the tiledata cache ...
  tiledataCache.flush();

  for(uint n : range(6)) {
    s.array(windowCache[n].above, 256);
    s.array(windowCache[n].below, 256);
  }

  for(uint n : range(4)) {
    s.integer(bg_info[n].tw);
    s.integer(bg_info[n].th);
    s.integer(bg_info[n].mx);
    s.integer(bg_info[n].my);
    s.integer(bg_info[n].scx);
    s.integer(bg_info[n].scy);
  }

  for(uint n : range(128)) {
    s.integer(obj.list[n].width);
    s.integer(obj.list[n].height);
    s.integer(obj.list[n].x);
    s.integer(obj.list[n].y);
    s.integer(obj.list[n].character);
    s.integer(obj.list[n].nameSelect);
    s.integer(obj.list[n].vflip);
    s.integer(obj.list[n].hflip);
    s.integer(obj.list[n].palette);
    s.integer(obj.list[n].priority);
    s.integer(obj.list[n].size);
  }
  s.integer(spriteListValid);
  s.integer(activeSprite);

  s.array(obj_itemList, 32);

  for(uint n : range(34)) {
    s.integer(obj_tileList[n].x);
    s.integer(obj_tileList[n].y);
    s.integer(obj_tileList[n].priority);
    s.integer(obj_tileList[n].palette);
    s.integer(obj_tileList[n].tile);
    s.integer(obj_tileList[n].hflip);
  }

  s.array(obj_linePalette, 256);
  s.array(obj_linePriority, 256);

  bg1.serialize(s);
  bg2.serialize(s);
  bg3.serialize(s);
  bg4.serialize(s);
  obj.serialize(s);
  window.serialize(s);
  screen.serialize(s);
}

auto PPU::Background::serialize(serializer& s) -> void {
  s.integer(r.tiledataAddress);
  s.integer(r.screenAddress);
  s.integer(r.screenSize);
  s.integer(r.mosaicEnabled);
  s.integer(r.tileSize);

  s.integer(r.aboveEnable);
  s.integer(r.belowEnable);

  s.integer(r.hoffset);
  s.integer(r.voffset);
}

auto PPU::Object::serialize(serializer& s) -> void {
  s.integer(r.aboveEnable);
  s.integer(r.belowEnable);
  s.integer(r.interlace);

  s.integer(r.baseSize);
  s.integer(r.nameSelect);
  s.integer(r.tiledataAddress);
  s.integer(r.firstSprite);

  s.integer(r.timeOver);
  s.integer(r.rangeOver);

  s.integer(t.itemCount);
  s.integer(t.tileCount);
}

auto PPU::Window::serialize(serializer& s) -> void {
  s.integer(r.bg1.oneEnable);
  s.integer(r.bg1.oneInvert);
  s.integer(r.bg1.twoEnable);
  s.integer(r.bg1.twoInvert);
  s.integer(r.bg1.mask);
  s.integer(r.bg1.aboveEnable);
  s.integer(r.bg1.belowEnable);
  
  s.integer(r.bg2.oneEnable);
  s.integer(r.bg2.oneInvert);
  s.integer(r.bg2.twoEnable);
  s.integer(r.bg2.twoInvert);
  s.integer(r.bg2.mask);
  s.integer(r.bg2.aboveEnable);
  s.integer(r.bg2.belowEnable);

  s.integer(r.bg3.oneEnable);
  s.integer(r.bg3.oneInvert);
  s.integer(r.bg3.twoEnable);
  s.integer(r.bg3.twoInvert);
  s.integer(r.bg3.mask);
  s.integer(r.bg3.aboveEnable);
  s.integer(r.bg3.belowEnable);

  s.integer(r.bg4.oneEnable);
  s.integer(r.bg4.oneInvert);
  s.integer(r.bg4.twoEnable);
  s.integer(r.bg4.twoInvert);
  s.integer(r.bg4.mask);
  s.integer(r.bg4.aboveEnable);
  s.integer(r.bg4.belowEnable);

  s.integer(r.obj.oneEnable);
  s.integer(r.obj.oneInvert);
  s.integer(r.obj.twoEnable);
  s.integer(r.obj.twoInvert);
  s.integer(r.obj.mask);
  s.integer(r.obj.aboveEnable);
  s.integer(r.obj.belowEnable);

  s.integer(r.col.oneEnable);
  s.integer(r.col.oneInvert);
  s.integer(r.col.twoEnable);
  s.integer(r.col.twoInvert);
  s.integer(r.col.mask);
  s.integer(r.col.aboveMask);
  s.integer(r.col.belowMask);

  s.integer(r.oneLeft);
  s.integer(r.oneRight);
  s.integer(r.twoLeft);
  s.integer(r.twoRight);
}

auto PPU::Screen::serialize(serializer& s) -> void {
  s.integer(r.blendMode);
  s.integer(r.directColor);

  s.integer(r.colorMode);
  s.integer(r.colorHalve);
  s.integer(r.bg1.colorEnable);
  s.integer(r.bg2.colorEnable);
  s.integer(r.bg3.colorEnable);
  s.integer(r.bg4.colorEnable);
  s.integer(r.obj.colorEnable);
  s.integer(r.back.colorEnable);

  s.integer(r.colorBlue);
  s.integer(r.colorGreen);
  s.integer(r.colorRed);

  //Not necessary?
  //s.integer(math.colorHalve);
}
