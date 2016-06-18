auto PPUcounter::serialize(serializer& s) -> void {
  s.integer(status.field);
  s.integer(status.hcounter);
  s.integer(status.vcounter);
}

auto PPU::serialize(serializer& s) -> void {
  Thread::serialize(s);
  PPUcounter::serialize(s);

  s.integer(status.chrAddressBus);
  s.integer(status.mdr);
  s.array(status.mdrDecay);

  s.integer(status.busData);

  s.integer(status.addressLatch);

  s.integer(status.vaddr);
  s.integer(status.taddr);
  s.integer(status.xaddr);

  s.integer(status.nmiHold);
  s.integer(status.nmiFlag);

  s.integer(status.nmiEnable);
  s.integer(status.masterSelect);
  s.integer(status.spriteSize);
  s.integer(status.bgAddress);
  s.integer(status.objAddress);
  s.integer(status.vramIncrement);

  s.integer(status.emphasis);
  s.integer(status.spriteEnable);
  s.integer(status.bgEnable);
  s.integer(status.objEdgeEnable);
  s.integer(status.bgEdgeEnable);
  s.integer(status.grayscale);

  s.integer(status.spriteZeroHit);
  s.integer(status.spriteOverflow);

  s.integer(status.oamAddress);

  s.integer(raster.nametable);
  s.integer(raster.attribute);
  s.integer(raster.tiledataLo);
  s.integer(raster.tiledataHi);

  s.integer(raster.oamIterator);
  s.integer(raster.oamCounter);

  for(uint n = 0; n < 8; n++) {
    s.integer(raster.oam[n].id);
    s.integer(raster.oam[n].y);
    s.integer(raster.oam[n].tile);
    s.integer(raster.oam[n].attr);
    s.integer(raster.oam[n].x);

    s.integer(raster.oam[n].tiledataLo);
    s.integer(raster.oam[n].tiledataHi);
  }

  for(uint n = 0; n < 8; n++) {
    s.integer(raster.soam[n].id);
    s.integer(raster.soam[n].y);
    s.integer(raster.soam[n].tile);
    s.integer(raster.soam[n].attr);
    s.integer(raster.soam[n].x);

    s.integer(raster.soam[n].tiledataLo);
    s.integer(raster.soam[n].tiledataHi);
  }

  s.array(ciram, !system.vs() ? 0x0800 : 0x1000);
  s.array(cgram);
  s.array(oam);
}
