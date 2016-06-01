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
bg1(*this, Background::ID::BG1),
bg2(*this, Background::ID::BG2),
bg3(*this, Background::ID::BG3),
bg4(*this, Background::ID::BG4),
sprite(*this),
window(*this),
screen(*this) {
  output = new uint32[512 * 512]();
  output += 16 * 512;  //overscan offset

  alloc_tiledata_cache();

  for(uint l : range(16)) {
    for(uint i : range(4096)) {
      mosaic_table[l][i] = (i / (l + 1)) * (l + 1);
    }
  }

  frameskip = 0;
  framecounter = 0;
}

PPU::~PPU() {
  output -= 16 * 512;
  delete[] output;
  free_tiledata_cache();
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
  cache.mode7_hoffset = regs.mode7_hoffset;
  cache.mode7_voffset = regs.mode7_voffset;
  cache.m7a = regs.m7a;
  cache.m7b = regs.m7b;
  cache.m7c = regs.m7c;
  cache.m7d = regs.m7d;
  cache.m7x = regs.m7x;
  cache.m7y = regs.m7y;
  if(vcounter() == vdisp()) {
    if(!regs.display_disable) {
      regs.oam_addr = regs.oam_baseaddr << 1;
      sprite.regs.first_sprite = !regs.oam_priority ? 0 : (regs.oam_addr >> 2) & 127;
    }
  }
  addClocks(502);

  //H =  512 (render)
  render_scanline();
  addClocks(640);

  //H = 1152 (cache OBSEL)
  if(cache.oam_basesize != sprite.regs.base_size) {
    cache.oam_basesize = sprite.regs.base_size;
    sprite_list_valid = false;
  }
  cache.oam_nameselect = sprite.regs.nameselect;
  cache.oam_tdaddr = sprite.regs.tiledata_addr;
  addClocks(lineclocks() - 1152);  //seek to start of next scanline
}

auto PPU::addClocks(uint clocks) -> void {
  tick(clocks);
  step(clocks);
  synchronizeCPU();
}

auto PPU::power() -> void {
  for(auto& n : vram) n = 0x00;
  for(auto& n : oam) n = 0x00;
  for(auto& n : cgram) n = 0x00;
  flush_tiledata_cache();

  regs.vram_readbuffer = 0x0000;
  regs.oam_latchdata = 0x00;
  regs.cgram_latchdata = 0x00;
  regs.bgofs_latchdata = 0x00;
  regs.mode7_latchdata = 0x00;
  regs.counters_latched = false;
  regs.latch_hcounter   = 0;
  regs.latch_vcounter   = 0;

  regs.oam_iaddr   = 0x0000;
  regs.cgram_iaddr = 0x01ff;

  //$2100  INIDISP
  regs.display_disable = true;
  regs.display_brightness = 15;

  //$2101
  sprite.regs.base_size     = 0;
  sprite.regs.nameselect    = 0;
  sprite.regs.tiledata_addr = 0x0000;

  cache.oam_basesize   = 0;
  cache.oam_nameselect = 0;
  cache.oam_tdaddr     = 0x0000;

  //$2102  OAMADDL
  //$2103  OAMADDH
  regs.oam_baseaddr        = 0x0000;
  regs.oam_addr            = 0x0000;
  regs.oam_priority        = false;
  sprite.regs.first_sprite = 0;

  //$2105  BGMODE
  bg1.regs.tile_size = Background::TileSize::Size8x8;
  bg2.regs.tile_size = Background::TileSize::Size8x8;
  bg3.regs.tile_size = Background::TileSize::Size8x8;
  bg4.regs.tile_size = Background::TileSize::Size8x8;
  regs.bg3_priority = false;
  regs.bgmode = 0;

  //$2106  MOSAIC
  regs.mosaic_size        = 0;
  bg1.regs.mosaic_enabled = false;
  bg2.regs.mosaic_enabled = false;
  bg3.regs.mosaic_enabled = false;
  bg4.regs.mosaic_enabled = false;
  regs.mosaic_countdown   = 0;

  //$2107  BG1SC
  //$2108  BG2SC
  //$2109  BG3SC
  //$210a  BG4SC
  bg1.regs.screen_addr = 0x0000;
  bg2.regs.screen_addr = 0x0000;
  bg3.regs.screen_addr = 0x0000;
  bg4.regs.screen_addr = 0x0000;
  bg1.regs.screen_size = Background::ScreenSize::Size32x32;
  bg2.regs.screen_size = Background::ScreenSize::Size32x32;
  bg3.regs.screen_size = Background::ScreenSize::Size32x32;
  bg4.regs.screen_size = Background::ScreenSize::Size32x32;

  //$210b  BG12NBA
  //$210c  BG34NBA
  bg1.regs.tiledata_addr = 0x0000;
  bg2.regs.tiledata_addr = 0x0000;
  bg3.regs.tiledata_addr = 0x0000;
  bg4.regs.tiledata_addr = 0x0000;

  //$210d  BG1HOFS
  bg1.regs.hoffset = 0x0000;
  regs.mode7_hoffset = 0x0000;

  //$210e  BG1VOFS
  bg1.regs.voffset = 0x0000;
  regs.mode7_voffset = 0x0000;

  //$210f  BG2HOFS
  bg2.regs.hoffset = 0x0000;

  //$2110  BG2VOFS
  bg2.regs.voffset = 0x0000;

  //$2111  BG3HOFS
  bg3.regs.hoffset = 0x0000;

  //$2112  BG3VOFS
  bg3.regs.voffset = 0x0000;

  //$2113  BG4HOFS
  bg4.regs.hoffset = 0x0000;

  //$2114  BG4VOFS
  bg4.regs.voffset = 0x0000;

  //$2115  VMAIN
  regs.vram_incmode = 1;
  regs.vram_mapping = 0;
  regs.vram_incsize = 1;

  //$2116  VMADDL
  //$2117  VMADDH
  regs.vram_addr = 0x0000;

  //$211a  M7SEL
  regs.mode7_repeat = 0;
  regs.mode7_vflip  = false;
  regs.mode7_hflip  = false;

  //$211b  M7A
  regs.m7a = 0x0000;

  //$211c  M7B
  regs.m7b = 0x0000;

  //$211d  M7C
  regs.m7c = 0x0000;

  //$211e  M7D
  regs.m7d = 0x0000;

  //$211f  M7X
  regs.m7x = 0x0000;

  //$2120  M7Y
  regs.m7y = 0x0000;

  //$2121  CGADD
  regs.cgram_addr = 0x0000;

  //$2123  W12SEL
  //$2124  W34SEL
  //$2125  WOBJSEL
  window.regs.bg1_one_enable = false;
  window.regs.bg2_one_enable = false;
  window.regs.bg3_one_enable = false;
  window.regs.bg4_one_enable = false;
  window.regs.oam_one_enable = false;
  window.regs.col_one_enable = false;

  window.regs.bg1_one_invert = false;
  window.regs.bg2_one_invert = false;
  window.regs.bg3_one_invert = false;
  window.regs.bg4_one_invert = false;
  window.regs.oam_one_invert = false;
  window.regs.col_one_invert = false;

  window.regs.bg1_two_enable = false;
  window.regs.bg2_two_enable = false;
  window.regs.bg3_two_enable = false;
  window.regs.bg4_two_enable = false;
  window.regs.oam_two_enable = false;
  window.regs.col_two_enable = false;

  window.regs.bg1_two_invert = false;
  window.regs.bg2_two_invert = false;
  window.regs.bg3_two_invert = false;
  window.regs.bg4_two_invert = false;
  window.regs.oam_two_invert = false;
  window.regs.col_two_invert = false;

  //$2126  WH0
  //$2127  WH1
  //$2128  WH2
  //$2129  WH3
  window.regs.one_left  = 0x00;
  window.regs.one_right = 0x00;
  window.regs.two_left  = 0x00;
  window.regs.two_right = 0x00;

  //$212a  WBGLOG
  //$212b  WOBJLOG
  window.regs.bg1_mask = 0;
  window.regs.bg2_mask = 0;
  window.regs.bg3_mask = 0;
  window.regs.bg4_mask = 0;
  window.regs.oam_mask = 0;
  window.regs.col_mask = 0;

  //$212c  TM
  //$212d  TS
  bg1.regs.main_enable    = false;
  bg2.regs.main_enable    = false;
  bg3.regs.main_enable    = false;
  bg4.regs.main_enable    = false;
  sprite.regs.main_enable = false;
  bg1.regs.sub_enable     = false;
  bg2.regs.sub_enable     = false;
  bg3.regs.sub_enable     = false;
  bg4.regs.sub_enable     = false;
  sprite.regs.sub_enable  = false;

  //$212e  TMW
  //$212f  TSW
  window.regs.bg1_main_enable = false;
  window.regs.bg2_main_enable = false;
  window.regs.bg3_main_enable = false;
  window.regs.bg4_main_enable = false;
  window.regs.oam_main_enable = false;
  window.regs.bg1_sub_enable  = false;
  window.regs.bg2_sub_enable  = false;
  window.regs.bg3_sub_enable  = false;
  window.regs.bg4_sub_enable  = false;
  window.regs.oam_sub_enable  = false;

  //$2130  CGWSEL
  window.regs.col_main_mask = 0;
  window.regs.col_sub_mask  = 0;
  screen.regs.addsub_mode   = false;
  screen.regs.direct_color  = false;

  //$2131  CGADDSUB
  screen.regs.color_mode        = 0;
  screen.regs.color_halve       = false;
  screen.regs.back_color_enable = false;
  screen.regs.oam_color_enable  = false;
  screen.regs.bg4_color_enable  = false;
  screen.regs.bg3_color_enable  = false;
  screen.regs.bg2_color_enable  = false;
  screen.regs.bg1_color_enable  = false;

  //$2132  COLDATA
  screen.regs.color_r = 0x00;
  screen.regs.color_g = 0x00;
  screen.regs.color_b = 0x00;
  regs.color_rgb      = 0x0000;

  //$2133  SETINI
  regs.mode7_extbg      = false;
  regs.pseudo_hires     = false;
  regs.overscan         = false;
  sprite.regs.interlace = false;
  regs.interlace        = false;

  //$213c  OPHCT
  regs.hcounter = 0;

  //$213d  OPVCT
  regs.vcounter = 0;

  //$213e  STAT77
  sprite.regs.time_over  = false;
  sprite.regs.range_over = false;

  reset();
}

auto PPU::reset() -> void {
  create(Enter, system.cpuFrequency());
  PPUcounter::reset();
  memory::fill(output, 512 * 480 * sizeof(uint32));

  function<auto (uint24, uint8) -> uint8> reader{&PPU::read, this};
  function<auto (uint24, uint8) -> void> writer{&PPU::write, this};
  bus.map(reader, writer, "00-3f,80-bf:2100-213f");

  frame();

  //$2100
  regs.display_disable = true;

  display.interlace = false;
  display.overscan  = false;

  memset(sprite_list, 0, sizeof(sprite_list));
  sprite_list_valid = false;

  //open bus support
  regs.ppu1_mdr = 0xff;
  regs.ppu2_mdr = 0xff;

  //bg line counters
  regs.bg_y[Background::ID::BG1] = 0;
  regs.bg_y[Background::ID::BG2] = 0;
  regs.bg_y[Background::ID::BG3] = 0;
  regs.bg_y[Background::ID::BG4] = 0;
}

auto PPU::scanline() -> void {
  line = vcounter();

  if(line == 0) {
    frame();

    //RTO flag reset
    sprite.regs.time_over  = false;
    sprite.regs.range_over = false;
  }

  if(line == 1) {
    //mosaic reset
    for(int bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) regs.bg_y[bg_id] = 1;
    regs.mosaic_countdown = regs.mosaic_size + 1;
    regs.mosaic_countdown--;
  } else {
    for(int bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) {
      auto bg = &bg1;
      switch(bg_id) {
      case Background::ID::BG1: bg = &bg1; break;
      case Background::ID::BG2: bg = &bg2; break;
      case Background::ID::BG3: bg = &bg3; break;
      case Background::ID::BG4: bg = &bg4; break;
      }
      if(!bg->regs.mosaic_enabled || !regs.mosaic_countdown) regs.bg_y[bg_id] = line;
    }
    if(!regs.mosaic_countdown) regs.mosaic_countdown = regs.mosaic_size + 1;
    regs.mosaic_countdown--;
  }

  if(line == 241) {
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::render_scanline() -> void {
  if(line >= 1 && line < 240) {
    if(framecounter) return;
    render_line();
  }
}

auto PPU::frame() -> void {
  if(field() == 0) display.interlace = regs.interlace;

  framecounter = (frameskip == 0 ? 0 : (framecounter + 1) % frameskip);
}

auto PPU::refresh() -> void {
  auto output = this->output;
  if(!overscan()) output -= 14 * 512;
  auto pitch = 512; //1024 >> interlace();
  auto width = 512;
  auto height = 480; //!interlace() ? 240 : 480;
  Emulator::video.refresh(output, pitch * sizeof(uint32), width, height);
}

auto PPU::set_frameskip(uint frameskip_) -> void {
  frameskip = frameskip_;
  framecounter = 0;
}

auto PPU::exportRegisters(string &markup) -> void {
  markup.append("ppu\n");
  //$2105  BGMODE
  markup.append("  bgmode:       ", regs.bgmode,       "\n");
  markup.append("  bg3-priority: ", regs.bg3_priority, "\n");
  //$2133  SETINI
  markup.append("  pseudo-hires: ", regs.pseudo_hires, "\n");
  markup.append("  overscan:     ", regs.overscan,     "\n");

  for(Background bg : (const Background[]){bg1, bg2, bg3, bg4}) {
    markup.append("  bg\n");
    markup.append("    tile-size:     ",   bg.regs.tile_size,              "\n");
    markup.append("    mosaic:        ",   bg.regs.mosaic_enabled ? (uint)regs.mosaic_size : 0, "\n");
    markup.append("    screen-addr:   0x", hex(bg.regs.screen_addr, 4L),   "\n");
    markup.append("    screen-size:   ",   bg.regs.screen_size,            "\n");
    markup.append("    tiledata-addr: 0x", hex(bg.regs.tiledata_addr, 4L), "\n");
    markup.append("    hoffset:       0x", hex(bg.regs.hoffset, 4L),       "\n");
    markup.append("    voffset:       0x", hex(bg.regs.voffset, 4L),       "\n");
    markup.append("    main-enable:   ",   bg.regs.main_enable,            "\n");
    markup.append("    sub-enable:    ",   bg.regs.sub_enable,             "\n");
  }
}

}
