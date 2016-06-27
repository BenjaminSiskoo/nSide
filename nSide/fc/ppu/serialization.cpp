auto PPUcounter::serialize(serializer& s) -> void {
  s.integer(status.field);
  s.integer(status.hcounter);
  s.integer(status.vcounter);
}

auto PPU::serialize(serializer& s) -> void {
  Thread::serialize(s);
  PPUcounter::serialize(s);

  s.integer(r.chrAddressBus);
  s.integer(r.mdr);
  s.array(r.mdrDecay);

  s.integer(r.busData);

  s.integer(r.addressLatch);

  s.integer(r.vaddr);
  s.integer(r.taddr);
  s.integer(r.xaddr);

  s.integer(r.nmiHold);
  s.integer(r.nmiFlag);

  s.integer(r.nmiEnable);
  s.integer(r.masterSelect);
  s.integer(r.spriteHeight);
  s.integer(r.bgAddress);
  s.integer(r.objAddress);
  s.integer(r.vramIncrement);

  s.integer(r.emphasis);
  s.integer(r.objEnable);
  s.integer(r.bgEnable);
  s.integer(r.objEdgeEnable);
  s.integer(r.bgEdgeEnable);
  s.integer(r.grayscale);

  s.integer(r.spriteZeroHit);
  s.integer(r.spriteOverflow);

  s.integer(r.oamAddress);

  s.integer(l.nametable);
  s.integer(l.attribute);
  s.integer(l.tiledataLo);
  s.integer(l.tiledataHi);

  s.integer(l.oamIterator);
  s.integer(l.oamCounter);

  for(uint n = 0; n < 8; n++) {
    s.integer(l.oam[n].id);
    s.integer(l.oam[n].y);
    s.integer(l.oam[n].tile);
    s.integer(l.oam[n].attr);
    s.integer(l.oam[n].x);

    s.integer(l.oam[n].tiledataLo);
    s.integer(l.oam[n].tiledataHi);
  }

  for(uint n = 0; n < 8; n++) {
    s.integer(l.soam[n].id);
    s.integer(l.soam[n].y);
    s.integer(l.soam[n].tile);
    s.integer(l.soam[n].attr);
    s.integer(l.soam[n].x);

    s.integer(l.soam[n].tiledataLo);
    s.integer(l.soam[n].tiledataHi);
  }

  s.array(ciram, !system.vs() ? 0x0800 : 0x1000);
  s.array(cgram);
  s.array(oam);
}
