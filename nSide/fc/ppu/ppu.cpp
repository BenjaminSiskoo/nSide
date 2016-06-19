#include <fc/fc.hpp>

namespace Famicom {

PPU ppu;

#include "memory.cpp"
#include "mmio.cpp"
#include "serialization.cpp"

PPU::PPU() {
  output = new uint32[256 * 312];
}

PPU::~PPU() {
  delete[] output;
}

auto PPU::step(uint clocks) -> void {
  clock += clocks;
}

auto PPU::synchronizeCPU() -> void {
  if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
}

auto PPU::Enter() -> void {
  while(true) scheduler.synchronize(), ppu.main();
}

auto PPU::main() -> void {
  rasterScanline();
}

auto PPU::addClocks(uint clocks) -> void {
  uint vbl = system.region() != System::Region::Dendy ? 241 : 291;
  uint pre = system.region() == System::Region::NTSC ? 261 : 311;

  while(clocks--) {
    if(vcounter() == vbl - 1 && hcounter() == 340) status.nmiHold = 1;

    if(vcounter() == vbl && hcounter() ==   0) status.chrAddressBus = status.vaddr & 0x3fff;
    if(vcounter() == vbl && hcounter() ==   0) status.nmiFlag = status.nmiHold;
    if(vcounter() == vbl && hcounter() ==   2) cpu.nmiLine(status.nmiEnable && status.nmiFlag);


    if(vcounter() == pre - 1 && hcounter() == 340) status.nmiHold = 0;
    if(vcounter() == pre     && hcounter() ==   0) status.nmiFlag = status.nmiHold;
    if(vcounter() == pre     && hcounter() ==   1) status.spriteZeroHit = 0, status.spriteOverflow = 0;
    if(vcounter() == pre     && hcounter() ==   2) cpu.nmiLine(status.nmiEnable && status.nmiFlag);

    step(system.region() == System::Region::NTSC ? 4 : 5);
    synchronizeCPU();

    for(uint i = 0; i < 8; i++) {
      if(--status.mdrDecay[i] == 0) status.mdr &= ~(1 << i);
    }

    tick(1);
  }
}

auto PPU::power() -> void {
  status.vaddr = 0x0000;

  status.nmiHold = 0;
  status.nmiFlag = 1;

  //$2003
  status.oamAddress = 0x00;

  for(auto& n : ciram) n = 0xff;
}

auto PPU::reset() -> void {
  create(PPU::Enter, system.cpuFrequency());
  PPUcounter::reset();
  memory::fill(output, 256 * 312 * sizeof(uint32));

  function<auto (uint16, uint8) -> uint8> reader{&PPU::read, this};
  function<auto (uint16, uint8) -> void> writer{&PPU::write, this};
  bus.map(reader, writer, "2000-3fff");

  status.mdr = 0x00;
  status.busData = 0x00;
  status.addressLatch = 0;

  status.taddr = 0x0000;
  status.xaddr = 0x00;

  //$2000
  status.nmiEnable = false;
  status.masterSelect = 0;
  status.spriteSize = 0;
  status.bgAddress = 0x0000;
  status.objAddress = 0x0000;
  status.vramIncrement = 1;

  //$2001
  status.emphasis = 0;
  status.spriteEnable = false;
  status.bgEnable = false;
  status.objEdgeEnable = false;
  status.bgEdgeEnable = false;
  status.grayscale = false;

  //$2002
  status.spriteZeroHit = false;
  status.spriteOverflow = false;

  for(auto& n : cgram) n = 0;
  for(auto& n : oam) n = 0;
}

auto PPU::scanline() -> void {
  if(vcounter() == 0) frame();
  cartridge.scanline(vcounter());

  if(vcounter() == 241) {
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::frame() -> void {
  //TODO: Verify whether putting the scheduler exit event at vcounter() == 241 reduces lag as opposed to here
}

auto PPU::origin_x() -> uint {
  return (system.vs() && interface->information.canvasWidth == 512) ? side * 256 : 0;
}

auto PPU::origin_y() -> uint {
  if(system.pc10()) {
    switch(playchoice10.screenConfig) {
    case PlayChoice10::ScreenConfig::Dual:   return 240;
    case PlayChoice10::ScreenConfig::Single: return   0;
    }
  } else {
    return 0;
  }
}

auto PPU::refresh() -> void {
  auto output = this->output;
  Emulator::video.refreshRegion(output, 256 * sizeof(uint32), origin_x(), origin_y(), 256, 240);
}

//

//vaddr = 0yyy VHYY  YYYX XXXX
//yyy = fine Yscroll (y:d0-d2)
//V = V nametable (y:d8)
//H = H nametable (x:d8)
//YYYYY = Y nametable (y:d3-d7)
//XXXXX = X nametable (x:d3-d7)

auto PPU::rasterEnable() const -> bool {
  return (status.bgEnable || status.spriteEnable);
}

auto PPU::nametableAddress() const -> uint {
  return 0x2000 + (status.vaddr & 0x0c00);
}

auto PPU::scrollX() const -> uint {
  return ((status.vaddr & 0x1f) << 3) | status.xaddr;
}

auto PPU::scrollY() const -> uint {
  return (((status.vaddr >> 5) & 0x1f) << 3) | ((status.vaddr >> 12) & 7);
}

auto PPU::spriteHeight() const -> uint {
  return status.spriteSize == 0 ? 8 : 16;
}

//

auto PPU::chrLoad(uint14 addr) -> uint8 {
  if(rasterEnable() == false) return 0x00;
  return cartridge.chrRead(status.chrAddressBus = addr);
}

//

auto PPU::scrollXIncrement() -> void {
  if(rasterEnable() == false) return;
  status.vaddr = (status.vaddr & 0x7fe0) | ((status.vaddr + 0x0001) & 0x001f);
  if((status.vaddr & 0x001f) == 0x0000) {
    status.vaddr ^= 0x0400;
  }
}

auto PPU::scrollYIncrement() -> void {
  if(rasterEnable() == false) return;
  status.vaddr = (status.vaddr & 0x0fff) | ((status.vaddr + 0x1000) & 0x7000);
  if((status.vaddr & 0x7000) == 0x0000) {
    status.vaddr = (status.vaddr & 0x7c1f) | ((status.vaddr + 0x0020) & 0x03e0);
    if((status.vaddr & 0x03e0) == 0x03c0) {  //0x03c0 == 30 << 5; 30 * 8 = 240
      status.vaddr &= 0x7c1f;
      status.vaddr ^= 0x0800;
    }
  }
}

//

auto PPU::ext() -> uint4 {
  return 0; //EXT pins are tied to ground
}

//

auto PPU::rasterPixel() -> void {
  uint lx = hcounter() - 1;
  uint16 mask = 0x8000 >> (status.xaddr + (lx & 7));
  uint8 palette = 0, objectPalette = 0;
  bool objectPriority = 0;
  palette |= (raster.tiledataLo & mask) ? 1 : 0;
  palette |= (raster.tiledataHi & mask) ? 2 : 0;
  if(palette) {
    uint16 attr = raster.attribute;
    if(mask >= 256) attr >>= 2;
    palette |= attr.bits(0,1) << 2;
  }

  if(status.bgEnable == false) palette = 0;
  if(status.bgEdgeEnable == false && lx < 8) palette = 0;

  if(status.spriteEnable == true) for(int sprite = 7; sprite >= 0; sprite--) {
    if(status.objEdgeEnable == false && lx < 8) continue;
    if(raster.oam[sprite].id == 64) continue;

    uint spritex = lx - raster.oam[sprite].x;
    if(spritex >= 8) continue;

    if(raster.oam[sprite].attr & 0x40) spritex ^= 7;
    uint mask = 0x80 >> spritex;
    uint spritePalette = 0;
    spritePalette |= (raster.oam[sprite].tiledataLo & mask) ? 1 : 0;
    spritePalette |= (raster.oam[sprite].tiledataHi & mask) ? 2 : 0;
    if(spritePalette == 0) continue;

    if(raster.oam[sprite].id == 0 && palette && lx != 255) status.spriteZeroHit = 1;
    spritePalette |= (raster.oam[sprite].attr & 3) << 2;

    objectPriority = raster.oam[sprite].attr & 0x20;
    objectPalette = 16 + spritePalette;
  }

  if(objectPalette) {
    if(palette == 0 || objectPriority == 0) palette = objectPalette;
  }

  if(revision >= Revision::RP2C07 && (lx < 2 || lx >= 254 || vcounter() <= 0)) {
    output[vcounter() * 256 + lx] = (status.emphasis << 6) | 0x1d;
    return;
  } else if(!rasterEnable()) {
    if((status.vaddr & 0x3f00) != 0x3f00) palette = ext();
    else palette = status.vaddr;
  } else {
    if(!palette) palette = ext();
  }
  output[vcounter() * 256 + lx] = (status.emphasis << 6) | cgramRead(palette);
}

auto PPU::rasterSprite() -> void {
  if(rasterEnable() == false) return;

  uint n = raster.oamIterator++;
  uint lastScanline = system.region() == System::Region::NTSC ? 261 : 311;
  int ly = vcounter() == lastScanline ? -1 : (int)vcounter();
  uint y = ly - oam[(n * 4) + 0];

  if(y >= spriteHeight()) return;
  if(raster.oamCounter == 8) {
    status.spriteOverflow = 1;
    return;
  }

  raster.soam[raster.oamCounter].id   = n;
  raster.soam[raster.oamCounter].y    = oam[(n * 4) + 0];
  raster.soam[raster.oamCounter].tile = oam[(n * 4) + 1];
  raster.soam[raster.oamCounter].attr = oam[(n * 4) + 2];
  raster.soam[raster.oamCounter].x    = oam[(n * 4) + 3];
  raster.oamCounter++;
}

auto PPU::rasterScanline() -> void {
  uint lastScanline = system.region() == System::Region::NTSC ? 261 : 311;
  if((vcounter() >= 240 && vcounter() < lastScanline)) {
    addClocks(341);
    return scanline();
  }

  raster.oamIterator = 0;
  raster.oamCounter = 0;

  for(uint n = 0; n < 8; n++) {
    raster.soam[n].id   = 64;
    raster.soam[n].y    = 0xff;
    raster.soam[n].tile = 0xff;
    raster.soam[n].attr = 0xff;
    raster.soam[n].x    = 0xff;
    raster.soam[n].tiledataLo = 0;
    raster.soam[n].tiledataHi = 0;
  }

  //  0
  addClocks(1);

  for(uint tile = 0; tile < 32; tile++) {  //  1-256
    uint nametable = chrLoad(0x2000 | (status.vaddr & 0x0fff));
    uint tileaddr = status.bgAddress + (nametable << 4) + (scrollY() & 7);
    rasterPixel();
    addClocks(1);

    rasterPixel();
    addClocks(1);

    uint attribute = chrLoad(0x23c0 | (status.vaddr & 0x0fc0) | ((scrollY() >> 5) << 3) | (scrollX() >> 5));
    if(scrollY() & 16) attribute >>= 4;
    if(scrollX() & 16) attribute >>= 2;
    rasterPixel();
    addClocks(1);

    scrollXIncrement();
    if(tile == 31) scrollYIncrement();
    rasterPixel();
    rasterSprite();
    addClocks(1);

    uint tiledataLo = chrLoad(tileaddr + 0);
    rasterPixel();
    addClocks(1);

    rasterPixel();
    addClocks(1);

    uint tiledataHi = chrLoad(tileaddr + 8);
    rasterPixel();
    addClocks(1);

    rasterPixel();
    rasterSprite();
    addClocks(1);

    raster.nametable = (raster.nametable << 8) | nametable;
    raster.attribute = (raster.attribute << 2) | (attribute & 3);
    raster.tiledataLo = (raster.tiledataLo << 8) | tiledataLo;
    raster.tiledataHi = (raster.tiledataHi << 8) | tiledataHi;
  }

  for(uint n = 0; n < 8; n++) raster.oam[n] = raster.soam[n];

  for(uint sprite = 0; sprite < 8; sprite++) {  //257-320
    uint nametable = chrLoad(0x2000 | (status.vaddr & 0x0fff));
    addClocks(1);

    if(rasterEnable() && sprite == 0) status.vaddr = (status.vaddr & 0x7be0) | (status.taddr & 0x041f);  //257
    addClocks(1);

    uint attribute = chrLoad(0x23c0 | (status.vaddr & 0x0fc0) | ((scrollY() >> 5) << 3) | (scrollX() >> 5));
    uint tileaddr = (spriteHeight() == 8)
    ? status.objAddress + raster.oam[sprite].tile * 16
    : ((raster.oam[sprite].tile & ~1) * 16) + ((raster.oam[sprite].tile & 1) * 0x1000);
    addClocks(2);

    uint spritey = (vcounter() - raster.oam[sprite].y) & (spriteHeight() - 1);
    if(raster.oam[sprite].attr & 0x80) spritey ^= (spriteHeight() - 1);
    tileaddr += spritey + (spritey & 8);

    raster.oam[sprite].tiledataLo = chrLoad(tileaddr + 0);
    addClocks(2);

    raster.oam[sprite].tiledataHi = chrLoad(tileaddr + 8);
    addClocks(2);

    if(rasterEnable() && sprite == 6 && vcounter() == lastScanline) status.vaddr = status.taddr;  //304
  }

  for(uint tile = 0; tile < 2; tile++) {  //321-336
    uint nametable = chrLoad(0x2000 | (status.vaddr & 0x0fff));
    uint tileaddr = status.bgAddress + (nametable << 4) + (scrollY() & 7);
    addClocks(2);

    uint attribute = chrLoad(0x23c0 | (status.vaddr & 0x0fc0) | ((scrollY() >> 5) << 3) | (scrollX() >> 5));
    if(scrollY() & 16) attribute >>= 4;
    if(scrollX() & 16) attribute >>= 2;
    addClocks(1);

    scrollXIncrement();
    addClocks(1);

    uint tiledataLo = chrLoad(tileaddr + 0);
    addClocks(2);

    uint tiledataHi = chrLoad(tileaddr + 8);
    addClocks(2);

    raster.nametable = (raster.nametable << 8) | nametable;
    raster.attribute = (raster.attribute << 2) | (attribute & 3);
    raster.tiledataLo = (raster.tiledataLo << 8) | tiledataLo;
    raster.tiledataHi = (raster.tiledataHi << 8) | tiledataHi;
  }

  //337-340
  chrLoad(0x2000 | (status.vaddr & 0x0fff));
  addClocks(2);

  chrLoad(0x2000 | (status.vaddr & 0x0fff));
  addClocks(1);
  if(hcounter() > 0) addClocks(1);

  return scanline();
}

// Arcade RGB palettes

const uint9 PPU::RP2C03[16 * 4] = {
  0333,0014,0006,0326,0403,0503,0510,0420,0320,0120,0031,0040,0022,0000,0000,0000,
  0555,0036,0027,0407,0507,0704,0700,0630,0430,0140,0040,0053,0044,0000,0000,0000,
  0777,0357,0447,0637,0707,0737,0740,0750,0660,0360,0070,0276,0077,0000,0000,0000,
  0777,0567,0657,0757,0747,0755,0764,0772,0773,0572,0473,0276,0467,0000,0000,0000,
};

const uint9 PPU::RP2C04_0001[16 * 4] = {
  0755,0637,0700,0447,0044,0120,0222,0704,0777,0333,0750,0503,0403,0660,0320,0777,
  0357,0653,0310,0360,0467,0657,0764,0027,0760,0276,0000,0200,0666,0444,0707,0014,
  0003,0567,0757,0070,0077,0022,0053,0507,0000,0420,0747,0510,0407,0006,0740,0000,
  0000,0140,0555,0031,0572,0326,0770,0630,0020,0036,0040,0111,0773,0737,0430,0473,
};

const uint9 PPU::RP2C04_0002[16 * 4] = {
  0000,0750,0430,0572,0473,0737,0044,0567,0700,0407,0773,0747,0777,0637,0467,0040,
  0020,0357,0510,0666,0053,0360,0200,0447,0222,0707,0003,0276,0657,0320,0000,0326,
  0403,0764,0740,0757,0036,0310,0555,0006,0507,0760,0333,0120,0027,0000,0660,0777,
  0653,0111,0070,0630,0022,0014,0704,0140,0000,0077,0420,0770,0755,0503,0031,0444,
};

const uint9 PPU::RP2C04_0003[16 * 4] = {
  0507,0737,0473,0555,0040,0777,0567,0120,0014,0000,0764,0320,0704,0666,0653,0467,
  0447,0044,0503,0027,0140,0430,0630,0053,0333,0326,0000,0006,0700,0510,0747,0755,
  0637,0020,0003,0770,0111,0750,0740,0777,0360,0403,0357,0707,0036,0444,0000,0310,
  0077,0200,0572,0757,0420,0070,0660,0222,0031,0000,0657,0773,0407,0276,0760,0022,
};

const uint9 PPU::RP2C04_0004[16 * 4] = {
  0430,0326,0044,0660,0000,0755,0014,0630,0555,0310,0070,0003,0764,0770,0040,0572,
  0737,0200,0027,0747,0000,0222,0510,0740,0653,0053,0447,0140,0403,0000,0473,0357,
  0503,0031,0420,0006,0407,0507,0333,0704,0022,0666,0036,0020,0111,0773,0444,0707,
  0757,0777,0320,0700,0760,0276,0777,0467,0000,0750,0637,0567,0360,0657,0077,0120,
};

}
