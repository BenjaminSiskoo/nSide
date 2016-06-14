#include <sfc-balanced/sfc.hpp>

namespace SuperFamicom {

PPU ppu;

#include "memory.cpp"
#include "mmio.cpp"
#include "background/background.cpp"
#include "screen/screen.cpp"
#include "sprite/sprite.cpp"
#include "window/window.cpp"
#include "render/render.cpp"
#include "serialization.cpp"

PPU::PPU() :
bg1(Background::ID::BG1),
bg2(Background::ID::BG2),
bg3(Background::ID::BG3),
bg4(Background::ID::BG4) {
  ppu1.version = 1;  //allowed values: 1
  ppu2.version = 3;  //allowed values: 1, 2, 3

  output = new uint32[512 * 512];
  output += 16 * 512;  //overscan offset

  alloc_tiledata_cache();

  for(uint l : range(16)) {
    for(uint i : range(4096)) {
      mosaicTable[l][i] = (i / (l + 1)) * (l + 1);
    }
  }
}

PPU::~PPU() {
  output -= 16 * 512;
  delete[] output;
  freeTiledataCache();
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
  //H =    0 (initialize)
  scanline();
  addClocks(10);

  //H =   10 (cache mode7 registers + OAM address reset)
  cache.hoffsetMode7 = r.hoffsetMode7;
  cache.voffsetMode7 = r.voffsetMode7;
  cache.m7a = r.m7a;
  cache.m7b = r.m7b;
  cache.m7c = r.m7c;
  cache.m7d = r.m7d;
  cache.m7x = r.m7x;
  cache.m7y = r.m7y;
  if(vcounter() == vdisp() && !r.displayDisable) oam.addressReset();
  addClocks(502);

  //H =  512 (render)
  renderScanline();
  addClocks(640);

  //H = 1152 (cache OBSEL)
  if(cache.oam_baseSize != oam.r.baseSize) {
    cache.oam_baseSize = oam.r.baseSize;
    spriteListValid = false;
  }
  cache.oam_nameSelect = oam.r.nameSelect;
  cache.oam_tiledataAddress = oam.r.tiledataAddress;
  addClocks(lineclocks() - 1152);  //seek to start of next scanline
}

auto PPU::addClocks(uint clocks) -> void {
  tick(clocks);
  step(clocks);
  synchronizeCPU();
}

auto PPU::power() -> void {
  for(auto& n : memory.vram) n = 0x00;
  for(auto& n : memory.oam) n = 0x00;
  for(auto& n : memory.cgram) n = 0x00;
  flushTiledataCache();
}

auto PPU::reset() -> void {
  create(Enter, system.cpuFrequency());
  PPUcounter::reset();
  memory::fill(output, 512 * 480 * sizeof(uint32));

  function<auto (uint24, uint8) -> uint8> reader{&PPU::read, this};
  function<auto (uint24, uint8) -> void> writer{&PPU::write, this};
  bus.map(reader, writer, "00-3f,80-bf:2100-213f");

  //open bus support
  ppu1.mdr = 0xff;
  ppu2.mdr = 0xff;

  latch.vram = 0x0000;
  latch.oam = 0x00;
  latch.cgram = 0x00;
  latch.bgofs = 0x00;
  latch.mode7 = 0x00;
  latch.counters = false;
  latch.hcounter = 0;
  latch.vcounter = 0;

  latch.oamAddress = 0x0000;
  latch.cgramAddress = 0x00;

  //$2100  INIDISP
  r.displayDisable = true;
  r.displayBrightness = 0;

  //$2101
  cache.oam_baseSize = 0;
  cache.oam_nameSelect = 0;
  cache.oam_tiledataAddress = 0x0000;

  //$2102  OAMADDL
  //$2103  OAMADDH
  r.oamBaseAddress = 0x0000;
  r.oamAddress = 0x0000;
  r.oamPriority = false;

  //$2105  BGMODE
  r.bgPriority = false;
  r.bgMode = 0;

  //$2106  MOSAIC
  r.mosaicSize = 0;
  r.mosaicCountdown = 0;

  //$210d  BG1HOFS
  r.hoffsetMode7 = 0x0000;

  //$210e  BG1VOFS
  r.voffsetMode7 = 0x0000;

  //$2115  VMAIN
  r.vramIncrementMode = 1;
  r.vramMapping = 0;
  r.vramIncrementSize = 1;

  //$2116  VMADDL
  //$2117  VMADDH
  r.vramAddress = 0x0000;

  //$211a  M7SEL
  r.repeatMode7 = 0;
  r.vflipMode7 = false;
  r.hflipMode7 = false;

  //$211b  M7A
  r.m7a = 0x0000;

  //$211c  M7B
  r.m7b = 0x0000;

  //$211d  M7C
  r.m7c = 0x0000;

  //$211e  M7D
  r.m7d = 0x0000;

  //$211f  M7X
  r.m7x = 0x0000;

  //$2120  M7Y
  r.m7y = 0x0000;

  //$2121  CGADD
  r.cgramAddress = 0x0000;

  //$2132  COLDATA
  r.color_rgb = 0x0000;

  //$2133  SETINI
  r.extbg = false;
  r.pseudoHires = false;
  r.overscan = false;
  r.interlace = false;

  //$213c  OPHCT
  r.hcounter = 0;

  //$213d  OPVCT
  r.vcounter = 0;

  bg1.reset();
  bg2.reset();
  bg3.reset();
  bg4.reset();
  oam.reset();
  window.reset();
  screen.reset();

  memset(spriteList, 0, sizeof(spriteList));
  spriteListValid = false;

  //bg line counters
  r.bg_y[Background::ID::BG1] = 0;
  r.bg_y[Background::ID::BG2] = 0;
  r.bg_y[Background::ID::BG3] = 0;
  r.bg_y[Background::ID::BG4] = 0;

  frame();
}

auto PPU::scanline() -> void {
  line = vcounter();

  if(line == 0) {
    frame();

    //RTO flag reset
    oam.r.timeOver  = false;
    oam.r.rangeOver = false;
  }

  if(line == 1) {
    //mosaic reset
    for(int bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) r.bg_y[bg_id] = 1;
    r.mosaicCountdown = r.mosaicSize + 1;
    r.mosaicCountdown--;
  } else {
    for(int bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) {
      auto bg = &bg1;
      switch(bg_id) {
      case Background::ID::BG1: bg = &bg1; break;
      case Background::ID::BG2: bg = &bg2; break;
      case Background::ID::BG3: bg = &bg3; break;
      case Background::ID::BG4: bg = &bg4; break;
      }
      if(!bg->r.mosaicEnabled || !r.mosaicCountdown) r.bg_y[bg_id] = line;
    }
    if(!r.mosaicCountdown) r.mosaicCountdown = r.mosaicSize + 1;
    r.mosaicCountdown--;
  }

  if(line == 241) {
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::renderScanline() -> void {
  if(line >= 1 && line < 240) {
    renderLine();
  }
}

auto PPU::frame() -> void {
  if(field() == 0) display.interlace = r.interlace;
}

auto PPU::refresh() -> void {
  auto output = this->output;
  if(!overscan()) output -= 14 * 512;
  auto pitch = 512; //1024 >> interlace();
  auto width = 512;
  auto height = 480; //!interlace() ? 240 : 480;
  Emulator::video.refresh(output, pitch * sizeof(uint32), width, height);
}

auto PPU::exportRegisters(string &markup) -> void {
  markup.append("ppu\n");
  //$2105  BGMODE
  markup.append("  bg-mode:      ", r.bgMode,     "\n");
  markup.append("  bg-priority:  ", r.bgPriority, "\n");
  //$2133  SETINI
  markup.append("  pseudo-hires: ", r.pseudoHires, "\n");
  markup.append("  overscan:     ", r.overscan,    "\n");

  for(Background bg : (const Background[]){bg1, bg2, bg3, bg4}) {
    markup.append("  bg\n");
    markup.append("    tile-size:        ",   bg.r.tileSize,                 "\n");
    markup.append("    mosaic:           ",   bg.r.mosaicEnabled ? (uint)r.mosaicSize : 0, "\n");
    markup.append("    screen-address:   0x", hex(bg.r.screenAddress, 4L),   "\n");
    markup.append("    screen-size:      ",   bg.r.screenSize,               "\n");
    markup.append("    tiledata-address: 0x", hex(bg.r.tiledataAddress, 4L), "\n");
    markup.append("    hoffset:          0x", hex(bg.r.hoffset, 4L),         "\n");
    markup.append("    voffset:          0x", hex(bg.r.voffset, 4L),         "\n");
    markup.append("    above-enable:     ",   bg.r.aboveEnable,              "\n");
    markup.append("    below-enable:     ",   bg.r.belowEnable,              "\n");
  }
}

}