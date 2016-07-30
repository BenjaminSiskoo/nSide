#include <sfc-balanced/sfc.hpp>

namespace SuperFamicom {

PPU ppu;

#include "io.cpp"
#include "background/background.cpp"
#include "object/object.cpp"
#include "window/window.cpp"
#include "screen/screen.cpp"
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

  tiledataCache.allocate();

  for(uint l : range(16)) {
    for(uint i : range(4096)) {
      mosaicTable[l][i] = (i / (l + 1)) * (l + 1);
    }
  }
}

PPU::~PPU() {
  output -= 16 * 512;
  delete[] output;
  tiledataCache.free();
}

auto PPU::step(uint clocks) -> void {
  tick(clocks);
  Thread::step(clocks);
  synchronize(cpu);
}

auto PPU::Enter() -> void {
  while(true) scheduler.synchronize(), ppu.main();
}

auto PPU::main() -> void {
  //H =    0 (initialize)
  scanline();
  step(10);

  //H =   10 (cache mode7 registers + OAM address reset)
  cache.hoffsetMode7 = io.hoffsetMode7;
  cache.voffsetMode7 = io.voffsetMode7;
  cache.m7a = io.m7a;
  cache.m7b = io.m7b;
  cache.m7c = io.m7c;
  cache.m7d = io.m7d;
  cache.m7x = io.m7x;
  cache.m7y = io.m7y;
  if(vcounter() == vdisp() && !io.displayDisable) obj.addressReset();
  step(502);

  //H =  512 (render)
  if(line == 0) obj_renderLine_rto();
  if(line >= 1 && line < 240) renderLine();
  step(640);

  //H = 1152 (cache OBSEL)
  if(cache.obj_baseSize != obj.io.baseSize) {
    cache.obj_baseSize = obj.io.baseSize;
  }
  cache.obj_nameselect = obj.io.nameselect;
  cache.obj_tiledataAddress = obj.io.tiledataAddress;
  step(lineclocks() - 1152);  //seek to start of next scanline
}

auto PPU::load(Markup::Node node) -> bool {
  ppu1.version = max(1, min(1, node["ppu1/version"].natural()));
  ppu2.version = max(1, min(3, node["ppu2/version"].natural()));
  ppu.vram.mask = node["ppu1/ram/size"].natural() - 1;
  if(ppu.vram.mask != 0xffff) ppu.vram.mask = 0x7fff;
  return true;
}

auto PPU::power() -> void {
  for(auto& n : vram.data) n = random(0x0000);
  tiledataCache.flush();
}

auto PPU::reset() -> void {
  create(Enter, system.colorburst() * 6.0);
  PPUcounter::reset();
  memory::fill(output, 512 * 480 * sizeof(uint32));

  function<auto (uint24, uint8) -> uint8> reader{&PPU::readIO, this};
  function<auto (uint24, uint8) -> void> writer{&PPU::writeIO, this};
  bus.map(reader, writer, "00-3f,80-bf:2100-213f");

  //open bus support
  ppu1.mdr = random(0xff);
  ppu2.mdr = random(0xff);

  latch.vram = random(0x0000);
  latch.oam = random(0x00);
  latch.cgram = random(0x00);
  latch.bgofs = random(0x00);
  latch.mode7 = random(0x00);
  latch.counters = false;
  latch.hcounter = 0;
  latch.vcounter = 0;

  latch.oamAddress = 0x0000;
  latch.cgramAddress = 0x00;

  //$2100  INIDISP
  io.displayDisable = true;
  io.displayBrightness = 0;

  //$2101
  cache.obj_baseSize = 0;
  cache.obj_nameselect = 0;
  cache.obj_tiledataAddress = 0x0000;

  //$2102  OAMADDL
  //$2103  OAMADDH
  io.oamBaseAddress = random(0x0000);
  io.oamAddress = random(0x0000);
  io.oamPriority = random(false);

  //$2105  BGMODE
  io.bgPriority = false;
  io.bgMode = 0;

  //$2106  MOSAIC
  io.mosaicCountdown = 0;

  //$210d  BG1HOFS
  io.hoffsetMode7 = random(0x0000);

  //$210e  BG1VOFS
  io.voffsetMode7 = random(0x0000);

  //$2115  VMAIN
  io.vramIncrementMode = random(1);
  io.vramMapping = random(0);
  io.vramIncrementSize = 1;

  //$2116  VMADDL
  //$2117  VMADDH
  io.vramAddress = random(0x0000);

  //$211a  M7SEL
  io.repeatMode7 = random(0);
  io.vflipMode7 = random(false);
  io.hflipMode7 = random(false);

  //$211b  M7A
  io.m7a = random(0x0000);

  //$211c  M7B
  io.m7b = random(0x0000);

  //$211d  M7C
  io.m7c = random(0x0000);

  //$211e  M7D
  io.m7d = random(0x0000);

  //$211f  M7X
  io.m7x = random(0x0000);

  //$2120  M7Y
  io.m7y = random(0x0000);

  //$2121  CGADD
  io.cgramAddress = random(0x00);
  io.cgramAddressLatch = random(0);

  //$2132  COLDATA
  io.color_rgb = 0x0000;

  //$2133  SETINI
  io.extbg = random(false);
  io.pseudoHires = random(false);
  io.overscan = false;
  io.interlace = false;

  //$213c  OPHCT
  io.hcounter = 0;

  //$213d  OPVCT
  io.vcounter = 0;

  bg1.reset();
  bg2.reset();
  bg3.reset();
  bg4.reset();
  obj.reset();
  window.reset();
  screen.reset();

  //bg line counters
  io.bg_y[Background::ID::BG1] = 0;
  io.bg_y[Background::ID::BG2] = 0;
  io.bg_y[Background::ID::BG3] = 0;
  io.bg_y[Background::ID::BG4] = 0;

  frame();
}

auto PPU::scanline() -> void {
  line = vcounter();

  if(line == 0) {
    frame();

    //RTO flag reset
    obj.io.timeOver  = false;
    obj.io.rangeOver = false;
  }

  if(line == 1) {
    //mosaic reset
    for(int bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) io.bg_y[bg_id] = 1;
    io.mosaicCountdown = max(bg1.io.mosaic, bg2.io.mosaic, bg3.io.mosaic, bg4.io.mosaic) + 1;
    io.mosaicCountdown--;
  } else {
    for(int bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) {
      auto bg = &bg1;
      switch(bg_id) {
      case Background::ID::BG1: bg = &bg1; break;
      case Background::ID::BG2: bg = &bg2; break;
      case Background::ID::BG3: bg = &bg3; break;
      case Background::ID::BG4: bg = &bg4; break;
      }
      if(!bg->io.mosaic || !io.mosaicCountdown) io.bg_y[bg_id] = line;
    }
    if(!io.mosaicCountdown) io.mosaicCountdown = max(bg1.io.mosaic, bg2.io.mosaic, bg3.io.mosaic, bg4.io.mosaic) + 1;
    io.mosaicCountdown--;
  }

  if(line == 241) {
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::frame() -> void {
  if(field() == 0) display.interlace = io.interlace;
}

auto PPU::refresh() -> void {
  auto output = this->output;
  if(!overscan()) output -= 14 * 512;
  auto pitch = 512; //1024 >> interlace();
  auto width = 512;
  auto height = 480; //!interlace() ? 240 : 480;
  Emulator::video.refresh(output, pitch * sizeof(uint32), width, height);
}

}
