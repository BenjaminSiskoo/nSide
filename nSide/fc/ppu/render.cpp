//vaddr = 0yyy VHYY  YYYX XXXX
//yyy = fine Yscroll (y:d0-d2)
//V = V nametable (y:d8)
//H = H nametable (x:d8)
//YYYYY = Y nametable (y:d3-d7)
//XXXXX = X nametable (x:d3-d7)

auto PPU::enable() const -> bool {
  return r.bgEnable || r.objEnable;
}

auto PPU::nametableAddress() const -> uint {
  return 0x2000 + (r.vaddr & 0x0c00);
}

auto PPU::scrollX() const -> uint {
  return ((r.vaddr & 0x1f) << 3) | r.xaddr;
}

auto PPU::scrollY() const -> uint {
  return (((r.vaddr >> 5) & 0x1f) << 3) | ((r.vaddr >> 12) & 7);
}

//

auto PPU::loadCHR(uint14 addr) -> uint8 {
  if(!enable()) return 0x00;
  return cartridge.readCHR(r.chrAddressBus = addr);
}

//

auto PPU::scrollX_increment() -> void {
  if(!enable()) return;
  r.vaddr = (r.vaddr & 0x7fe0) | ((r.vaddr + 0x0001) & 0x001f);
  if((r.vaddr & 0x001f) == 0x0000) {
    r.vaddr ^= 0x0400;
  }
}

auto PPU::scrollY_increment() -> void {
  if(!enable()) return;
  r.vaddr = (r.vaddr & 0x0fff) | ((r.vaddr + 0x1000) & 0x7000);
  if((r.vaddr & 0x7000) == 0x0000) {
    r.vaddr = (r.vaddr & 0x7c1f) | ((r.vaddr + 0x0020) & 0x03e0);
    if((r.vaddr & 0x03e0) == 0x03c0) {  //0x03c0 == 30 << 5; 30 * 8 = 240
      r.vaddr &= 0x7c1f;
      r.vaddr ^= 0x0800;
    }
  }
}

//

auto PPU::renderPixel() -> void {
  uint lx = hcounter() - 1;
  uint mask = 0x8000 >> (r.xaddr + (lx & 7));
  uint palette = 0;
  uint objectPalette = 0;
  bool objectPriority = 0;
  palette |= (l.tiledataLo & mask) ? 1 : 0;
  palette |= (l.tiledataHi & mask) ? 2 : 0;
  if(palette) {
    uint attr = l.attribute;
    if(mask >= 256) attr >>= 2;
    palette |= (attr & 3) << 2;
  }

  if(!r.bgEnable) palette = 0;
  if(!r.bgEdgeEnable && lx < 8) palette = 0;

  if(r.objEnable)
  for(int sprite = 7; sprite >= 0; sprite--) {
    if(!r.objEdgeEnable && lx < 8) continue;
    if(l.oam[sprite].id == 64) continue;

    uint spriteX = lx - l.oam[sprite].x;
    if(spriteX >= 8) continue;

    if(l.oam[sprite].attr & 0x40) spriteX ^= 7;
    uint mask = 0x80 >> spriteX;
    uint spritePalette = 0;
    spritePalette |= (l.oam[sprite].tiledataLo & mask) ? 1 : 0;
    spritePalette |= (l.oam[sprite].tiledataHi & mask) ? 2 : 0;
    if(spritePalette == 0) continue;

    if(l.oam[sprite].id == 0 && palette && lx != 255) r.spriteZeroHit = 1;
    spritePalette |= (l.oam[sprite].attr & 3) << 2;

    objectPriority = l.oam[sprite].attr & 0x20;
    objectPalette = 16 + spritePalette;
  }

  if(objectPalette) {
    if(palette == 0 || objectPriority == 0) palette = objectPalette;
  }

  if(version >= Version::RP2C07 && (lx < 2 || lx >= 254 || vcounter() <= 0)) {
    output[vcounter() * 256 + lx] = (r.emphasis << 6) | 0x1d;
    return;
  } else if(!enable()) {
    if((r.vaddr & 0x3f00) != 0x3f00) palette = ext();
    else palette = r.vaddr;
  } else {
    if(!palette) palette = ext();
  }
  output[vcounter() * 256 + lx] = r.emphasis << 6 | readCGRAM(palette);
}

auto PPU::renderSprite() -> void {
  if(!enable()) return;

  uint n = l.oamIterator++;
  uint lastScanline = system.region() == System::Region::NTSC ? 261 : 311;
  int ly = vcounter() == lastScanline ? -1 : (int)vcounter();
  uint y = ly - oam[(n * 4) + 0];

  if(y >= r.spriteHeight) return;
  if(l.oamCounter == 8) {
    r.spriteOverflow = 1;
    return;
  }

  auto& o = l.soam[l.oamCounter++];
  o.id   = n;
  o.y    = oam[(n * 4) + 0];
  o.tile = oam[(n * 4) + 1];
  o.attr = oam[(n * 4) + 2];
  o.x    = oam[(n * 4) + 3];
}

auto PPU::renderScanline() -> void {
  uint lastScanline = system.region() == System::Region::NTSC ? 261 : 311;
  if((vcounter() >= 240 && vcounter() < lastScanline)) {
    addClocks(341);
    return scanline();
  }

  l.oamIterator = 0;
  l.oamCounter = 0;

  for(uint n = 0; n < 8; n++) {
    l.soam[n].id   = 64;
    l.soam[n].y    = 0xff;
    l.soam[n].tile = 0xff;
    l.soam[n].attr = 0xff;
    l.soam[n].x    = 0xff;
    l.soam[n].tiledataLo = 0;
    l.soam[n].tiledataHi = 0;
  }

  //  0
  addClocks(1);

  for(uint tile = 0; tile < 32; tile++) {  //  1-256
    uint nametable = loadCHR(0x2000 | (r.vaddr & 0x0fff));
    uint tileaddr = r.bgAddress + (nametable << 4) + (scrollY() & 7);
    renderPixel();
    addClocks(1);

    renderPixel();
    addClocks(1);

    uint attribute = loadCHR(0x23c0 | (r.vaddr & 0x0fc0) | ((scrollY() >> 5) << 3) | (scrollX() >> 5));
    if(scrollY() & 16) attribute >>= 4;
    if(scrollX() & 16) attribute >>= 2;
    renderPixel();
    addClocks(1);

    scrollX_increment();
    if(tile == 31) scrollY_increment();
    renderPixel();
    renderSprite();
    addClocks(1);

    uint tiledataLo = loadCHR(tileaddr + 0);
    renderPixel();
    addClocks(1);

    renderPixel();
    addClocks(1);

    uint tiledataHi = loadCHR(tileaddr + 8);
    renderPixel();
    addClocks(1);

    renderPixel();
    renderSprite();
    addClocks(1);

    l.nametable = (l.nametable << 8) | nametable;
    l.attribute = (l.attribute << 2) | (attribute & 3);
    l.tiledataLo = (l.tiledataLo << 8) | tiledataLo;
    l.tiledataHi = (l.tiledataHi << 8) | tiledataHi;
  }

  for(uint n = 0; n < 8; n++) l.oam[n] = l.soam[n];

  for(uint sprite = 0; sprite < 8; sprite++) {  //257-320
    uint nametable = loadCHR(0x2000 | (r.vaddr & 0x0fff));
    addClocks(1);

    if(enable() && sprite == 0) r.vaddr = (r.vaddr & 0x7be0) | (r.taddr & 0x041f);  //257
    addClocks(1);

    uint attribute = loadCHR(0x23c0 | (r.vaddr & 0x0fc0) | ((scrollY() >> 5) << 3) | (scrollX() >> 5));
    uint tileaddr = r.spriteHeight == 8
    ? r.objAddress + l.oam[sprite].tile * 16
    : ((l.oam[sprite].tile & ~1) * 16) + ((l.oam[sprite].tile & 1) * 0x1000);
    addClocks(2);

    uint spriteY = (vcounter() - l.oam[sprite].y) & (r.spriteHeight - 1);
    if(l.oam[sprite].attr & 0x80) spriteY ^= (r.spriteHeight - 1);
    tileaddr += spriteY + (spriteY & 8);

    l.oam[sprite].tiledataLo = loadCHR(tileaddr + 0);
    addClocks(2);

    l.oam[sprite].tiledataHi = loadCHR(tileaddr + 8);
    addClocks(2);

    if(enable() && sprite == 6 && vcounter() == lastScanline) r.vaddr = r.taddr;  //304
  }

  for(uint tile = 0; tile < 2; tile++) {  //321-336
    uint nametable = loadCHR(0x2000 | (r.vaddr & 0x0fff));
    uint tileaddr = r.bgAddress + (nametable << 4) + (scrollY() & 7);
    addClocks(2);

    uint attribute = loadCHR(0x23c0 | (r.vaddr & 0x0fc0) | ((scrollY() >> 5) << 3) | (scrollX() >> 5));
    if(scrollY() & 16) attribute >>= 4;
    if(scrollX() & 16) attribute >>= 2;
    addClocks(1);

    scrollX_increment();
    addClocks(1);

    uint tiledataLo = loadCHR(tileaddr + 0);
    addClocks(2);

    uint tiledataHi = loadCHR(tileaddr + 8);
    addClocks(2);

    l.nametable = (l.nametable << 8) | nametable;
    l.attribute = (l.attribute << 2) | (attribute & 3);
    l.tiledataLo = (l.tiledataLo << 8) | tiledataLo;
    l.tiledataHi = (l.tiledataHi << 8) | tiledataHi;
  }

  //337-340
  loadCHR(0x2000 | (r.vaddr & 0x0fff));
  addClocks(2);

  loadCHR(0x2000 | (r.vaddr & 0x0fff));
  addClocks(1);
  if(hcounter() > 0) addClocks(1);

  return scanline();
}
