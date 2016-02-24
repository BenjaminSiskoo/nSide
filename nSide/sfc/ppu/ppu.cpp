#include <sfc/sfc.hpp>

namespace SuperFamicom {

PPU ppu;
#include "video.cpp"

#include "background/background.cpp"
#include "mmio/mmio.cpp"
#include "screen/screen.cpp"
#include "sprite/sprite.cpp"
#include "window/window.cpp"
#include "serialization.cpp"

PPU::PPU() :
bg1(*this, Background::ID::BG1),
bg2(*this, Background::ID::BG2),
bg3(*this, Background::ID::BG3),
bg4(*this, Background::ID::BG4),
sprite(*this),
window(*this),
screen(*this) {
  output = new uint32[512 * 512];
}

PPU::~PPU() {
  delete[] output;
}

auto PPU::step(uint clocks) -> void {
  clock += clocks;
}

auto PPU::synchronizeCPU() -> void {
  if(CPU::Threaded) {
    if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
  } else {
    while(clock >= 0) cpu.main();
  }
}

auto PPU::Enter() -> void {
  while(true) scheduler.synchronize(), ppu.main();
}

auto PPU::main() -> void {
  scanline();
  add_clocks(28);
  bg1.begin();
  bg2.begin();
  bg3.begin();
  bg4.begin();

  if(vcounter() <= 239) {
    for(int pixel = -7; pixel <= 255; pixel++) {
      bg1.run(1);
      bg2.run(1);
      bg3.run(1);
      bg4.run(1);
      add_clocks(2);

      bg1.run(0);
      bg2.run(0);
      bg3.run(0);
      bg4.run(0);
      if(pixel >= 0) {
        sprite.run();
        window.run();
        screen.run();
      }
      add_clocks(2);
    }

    add_clocks(14);
    sprite.tilefetch();
  } else {
    add_clocks(1052 + 14 + 136);
  }

  add_clocks(lineclocks() - 28 - 1052 - 14 - 136);
}

auto PPU::add_clocks(uint clocks) -> void {
  clocks >>= 1;
  while(clocks--) {
    tick(2);
    step(2);
    synchronizeCPU();
  }
}

auto PPU::enable() -> void {
  function<auto (uint, uint8) -> uint8> reader{&PPU::mmio_read, (PPU*)&ppu};
  function<auto (uint, uint8) -> void> writer{&PPU::mmio_write, (PPU*)&ppu};

  bus.map(reader, writer, 0x00, 0x3f, 0x2100, 0x213f);
  bus.map(reader, writer, 0x80, 0xbf, 0x2100, 0x213f);
}

auto PPU::power() -> void {
  for(auto& n : vram) n = random(0x00);
  for(auto& n : oam) n = random(0x00);
  for(auto& n : cgram) n = random(0x00);
}

auto PPU::reset() -> void {
  create(Enter, system.cpuFrequency());
  PPUcounter::reset();
  memory::fill(output, 512 * 480 * sizeof(uint32));

  mmio_reset();
  bg1.reset();
  bg2.reset();
  bg3.reset();
  bg4.reset();
  sprite.reset();
  window.reset();
  screen.reset();
  video.reset();

  frame();
}

auto PPU::scanline() -> void {
  if(vcounter() == 0) {
    frame();
    bg1.frame();
    bg2.frame();
    bg3.frame();
    bg4.frame();
  }

  bg1.scanline();
  bg2.scanline();
  bg3.scanline();
  bg4.scanline();
  sprite.scanline();
  window.scanline();
  screen.scanline();

  if(vcounter() == 241) {
    video.refresh();
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::frame() -> void {
  sprite.frame();

  display.interlace = regs.interlace;
  display.overscan = regs.overscan;
}

auto PPU::exportRegisters(string &markup) -> void {
  markup.append("ppu\n");
  // 2105
  markup.append("  bgmode:       ", regs.bgmode,       "\n");
  markup.append("  bg3-priority: ", regs.bg3_priority, "\n");
  // 2133
  markup.append("  pseudo-hires: ", regs.pseudo_hires, "\n");
  markup.append("  overscan:     ", regs.overscan,     "\n");
  // individual backgrounds
  auto bg = &bg1;
  for(uint bg_id = 1; bg_id <= 4; bg_id++) {
    switch(bg_id) {
      case 1: bg = &bg1; break;
      case 2: bg = &bg2; break;
      case 3: bg = &bg3; break;
      case 4: bg = &bg4; break;
    }
    markup.append("  bg\n");
    markup.append("    tile-size:     ",   bg->regs.tile_size,              "\n");
    markup.append("    mosaic:        ",   bg->regs.mosaic,                 "\n");
    markup.append("    screen-addr:   0x", hex(bg->regs.screen_addr, 4L),   "\n");
    markup.append("    screen-size:   ",   bg->regs.screen_size,            "\n");
    markup.append("    tiledata-addr: 0x", hex(bg->regs.tiledata_addr, 4L), "\n");
    markup.append("    hoffset:       0x", hex(bg->regs.hoffset, 3L),       "\n");
    markup.append("    voffset:       0x", hex(bg->regs.voffset, 3L),       "\n");
    markup.append("    main-enable:   ",   bg->regs.main_enable,            "\n");
    markup.append("    sub-enable:    ",   bg->regs.sub_enable,             "\n");
  }
}

}
