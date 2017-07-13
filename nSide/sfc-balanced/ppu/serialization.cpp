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

  s.integer(ppu1.version);
  s.integer(ppu1.mdr);

  s.integer(ppu2.version);
  s.integer(ppu2.mdr);

  s.integer(line);

  s.integer(display.interlace);
  s.integer(display.overscan);

  s.integer(cache.obj_baseSize);
  s.integer(cache.obj_nameselect);
  s.integer(cache.obj_tiledataAddress);

  s.array(io.bg_y);

  s.integer(latch.vram);
  s.integer(latch.oam);
  s.integer(latch.cgram);
  s.integer(latch.bgofsPPU1);
  s.integer(latch.bgofsPPU2);
  s.integer(latch.mode7);
  s.integer(latch.counters);
  s.integer(latch.hcounter);
  s.integer(latch.vcounter);

  s.integer(latch.oamAddress);
  s.integer(latch.cgramAddress);

  s.integer(io.displayDisable);
  s.integer(io.displayBrightness);

  s.integer(io.oamBaseAddress);
  s.integer(io.oamAddress);
  s.integer(io.oamPriority);

  s.integer(io.bgPriority);
  s.integer(io.bgMode);

  s.integer(io.mosaicCountdown);

  s.integer(io.hoffsetMode7);
  s.integer(io.voffsetMode7);

  s.integer(io.vramIncrementMode);
  s.integer(io.vramMapping);
  s.integer(io.vramIncrementSize);

  s.integer(io.vramAddress);

  s.integer(io.repeatMode7);
  s.integer(io.vflipMode7);
  s.integer(io.hflipMode7);

  s.integer(io.m7a);
  s.integer(io.m7b);
  s.integer(io.m7c);
  s.integer(io.m7d);
  s.integer(io.m7x);
  s.integer(io.m7y);

  s.integer(io.cgramAddress);
  s.integer(io.cgramAddressLatch);

  s.integer(io.color_rgb);

  s.integer(io.extbg);
  s.integer(io.pseudoHires);
  s.integer(io.overscan);
  s.integer(io.interlace);

  s.integer(io.hcounter);
  s.integer(io.vcounter);

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
  s.integer(io.tiledataAddress);
  s.integer(io.screenAddress);
  s.integer(io.screenSize);
  s.integer(io.mosaic);
  s.integer(io.tileSize);

  s.integer(io.mode);
  s.array(io.priority);

  s.integer(io.aboveEnable);
  s.integer(io.belowEnable);

  s.integer(io.hoffset);
  s.integer(io.voffset);

  //s.integer(x);
  s.integer(y);

  //s.integer(tileCounter);
  s.integer(tile);
  s.integer(priority);
  s.integer(paletteNumber);
  s.integer(paletteIndex);
  //s.array(data);
}

auto PPU::Object::serialize(serializer& s) -> void {
  for(auto& object : oam.object) {
    s.integer(object.x);
    s.integer(object.y);
    s.integer(object.character);
    s.integer(object.nameselect);
    s.integer(object.vflip);
    s.integer(object.hflip);
    s.integer(object.priority);
    s.integer(object.palette);
    s.integer(object.size);
  }

  s.integer(io.aboveEnable);
  s.integer(io.belowEnable);
  s.integer(io.interlace);

  s.integer(io.baseSize);
  s.integer(io.nameselect);
  s.integer(io.tiledataAddress);
  s.integer(io.firstSprite);

  s.array(io.priority);

  s.integer(io.timeOver);
  s.integer(io.rangeOver);

  s.integer(t.itemCount);
  s.integer(t.tileCount);
}

auto PPU::Window::serialize(serializer& s) -> void {
  s.integer(io.bg1.oneEnable);
  s.integer(io.bg1.oneInvert);
  s.integer(io.bg1.twoEnable);
  s.integer(io.bg1.twoInvert);
  s.integer(io.bg1.mask);
  s.integer(io.bg1.aboveEnable);
  s.integer(io.bg1.belowEnable);
  
  s.integer(io.bg2.oneEnable);
  s.integer(io.bg2.oneInvert);
  s.integer(io.bg2.twoEnable);
  s.integer(io.bg2.twoInvert);
  s.integer(io.bg2.mask);
  s.integer(io.bg2.aboveEnable);
  s.integer(io.bg2.belowEnable);

  s.integer(io.bg3.oneEnable);
  s.integer(io.bg3.oneInvert);
  s.integer(io.bg3.twoEnable);
  s.integer(io.bg3.twoInvert);
  s.integer(io.bg3.mask);
  s.integer(io.bg3.aboveEnable);
  s.integer(io.bg3.belowEnable);

  s.integer(io.bg4.oneEnable);
  s.integer(io.bg4.oneInvert);
  s.integer(io.bg4.twoEnable);
  s.integer(io.bg4.twoInvert);
  s.integer(io.bg4.mask);
  s.integer(io.bg4.aboveEnable);
  s.integer(io.bg4.belowEnable);

  s.integer(io.obj.oneEnable);
  s.integer(io.obj.oneInvert);
  s.integer(io.obj.twoEnable);
  s.integer(io.obj.twoInvert);
  s.integer(io.obj.mask);
  s.integer(io.obj.aboveEnable);
  s.integer(io.obj.belowEnable);

  s.integer(io.col.oneEnable);
  s.integer(io.col.oneInvert);
  s.integer(io.col.twoEnable);
  s.integer(io.col.twoInvert);
  s.integer(io.col.mask);
  s.integer(io.col.aboveMask);
  s.integer(io.col.belowMask);

  s.integer(io.oneLeft);
  s.integer(io.oneRight);
  s.integer(io.twoLeft);
  s.integer(io.twoRight);
}

auto PPU::Screen::serialize(serializer& s) -> void {
  s.array(cgram);

  s.integer(io.blendMode);
  s.integer(io.directColor);

  s.integer(io.colorMode);
  s.integer(io.colorHalve);
  s.integer(io.bg1.colorEnable);
  s.integer(io.bg2.colorEnable);
  s.integer(io.bg3.colorEnable);
  s.integer(io.bg4.colorEnable);
  s.integer(io.obj.colorEnable);
  s.integer(io.back.colorEnable);

  s.integer(io.colorBlue);
  s.integer(io.colorGreen);
  s.integer(io.colorRed);

  s.integer(math.colorHalve);
}
