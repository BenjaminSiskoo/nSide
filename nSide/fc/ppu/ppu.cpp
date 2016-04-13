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
  raster_scanline();
}

auto PPU::addClocks(uint clocks) -> void {
  uint vbl;
  uint pre;
  switch(system.region()) {
  case System::Region::NTSC:  vbl = 241; pre = 261; break;
  case System::Region::PAL:   vbl = 241; pre = 311; break;
//case System::Region::Dendy: vbl = 291; pre = 311; break;
  }

  while(clocks--) {
    if(vcounter() == 240 && hcounter() == 340) status.nmi_hold = 1;

    if(vcounter() == vbl && hcounter() ==   0) status.chr_abus = status.vaddr & 0x3fff;
    if(vcounter() == vbl && hcounter() ==   0) status.nmi_flag = status.nmi_hold;
    if(vcounter() == vbl && hcounter() ==   2) cpu.set_nmi_line(status.nmi_enable && status.nmi_flag);


    if(vcounter() == pre - 1 && hcounter() == 340) status.nmi_hold = 0;
    if(vcounter() == pre     && hcounter() ==   0) status.nmi_flag = status.nmi_hold;
    if(vcounter() == pre     && hcounter() ==   1) status.sprite_zero_hit = 0, status.sprite_overflow = 0;
    if(vcounter() == pre     && hcounter() ==   2) cpu.set_nmi_line(status.nmi_enable && status.nmi_flag);

    step(system.region() == System::Region::NTSC ? 4 : 5);
    synchronizeCPU();

    for(uint i = 0; i < 8; i++) {
      if(--status.mdr_decay[i] == 0) status.mdr &= ~(1 << i);
    }

    tick(1);
  }
}

auto PPU::power() -> void {
  status.vaddr = 0x0000;

  status.nmi_hold = 0;
  status.nmi_flag = 1;

  //$2003
  status.oam_addr = 0x00;

  for(auto& n : ciram  ) n = 0xff;
}

auto PPU::reset() -> void {
  create(PPU::Enter, system.cpuFrequency());
  PPUcounter::reset();
  memory::fill(output, 256 * 312 * sizeof(uint32));

  function<auto (uint16, uint8) -> uint8> reader{&PPU::read, this};
  function<auto (uint16, uint8) -> void> writer{&PPU::write, this};
  bus.map(reader, writer, "2000-3fff");

  status.mdr = 0x00;
  status.bus_data = 0x00;
  status.address_latch = 0;

  status.taddr = 0x0000;
  status.xaddr = 0x00;

  //$2000
  status.nmi_enable = false;
  status.master_select = 0;
  status.sprite_size = 0;
  status.bg_addr = 0x0000;
  status.sprite_addr = 0x0000;
  status.vram_increment = 1;

  //$2001
  status.emphasis = 0;
  status.sprite_enable = false;
  status.bg_enable = false;
  status.sprite_edge_enable = false;
  status.bg_edge_enable = false;
  status.grayscale = false;

  //$2002
  status.sprite_zero_hit = false;
  status.sprite_overflow = false;

  for(auto& n : cgram  ) n = 0;
  for(auto& n : oam    ) n = 0;
}

auto PPU::origin_x() -> uint {
  return (system.vs() && interface->information.width == 512) ? side * 256 : 0;
}

auto PPU::origin_y() -> uint {
  if(system.pc10()) {
    switch(playchoice10.screenConfig) {
    case PlayChoice10::ScreenConfig::Dual:
      return 240;
    case PlayChoice10::ScreenConfig::Single:
      return 0;
    }
  } else {
    return 0;
  }
}

auto PPU::scanline() -> void {
  if(vcounter() == 0) frame();
  cartridge.scanline(vcounter());

  if(vcounter() == 241) {
    auto output = this->output;
    Emulator::video.refreshRegion(output, 256 * sizeof(uint32), origin_x(), origin_y(), 256, 240);
    scheduler.exit(Scheduler::Event::Frame);
  }
}

auto PPU::frame() -> void {
  if(system.pc10()) playchoice10.videoCircuit.update();
}

//

//vaddr = 0yyy VHYY  YYYX XXXX
//yyy = fine Yscroll (y:d0-d2)
//V = V nametable (y:d8)
//H = H nametable (x:d8)
//YYYYY = Y nametable (y:d3-d7)
//XXXXX = X nametable (x:d3-d7)

auto PPU::raster_enable() const -> bool {
  return (status.bg_enable || status.sprite_enable);
}

auto PPU::nametable_addr() const -> uint {
  return 0x2000 + (status.vaddr & 0x0c00);
}

auto PPU::scrollx() const -> uint {
  return ((status.vaddr & 0x1f) << 3) | status.xaddr;
}

auto PPU::scrolly() const -> uint {
  return (((status.vaddr >> 5) & 0x1f) << 3) | ((status.vaddr >> 12) & 7);
}

auto PPU::sprite_height() const -> uint {
  return status.sprite_size == 0 ? 8 : 16;
}

//

auto PPU::chr_load(uint14 addr) -> uint8 {
  if(raster_enable() == false) return 0x00;
  return cartridge.chr_read(status.chr_abus = addr);
}

//

auto PPU::scrollx_increment() -> void {
  if(raster_enable() == false) return;
  status.vaddr = (status.vaddr & 0x7fe0) | ((status.vaddr + 0x0001) & 0x001f);
  if((status.vaddr & 0x001f) == 0x0000) {
    status.vaddr ^= 0x0400;
  }
}

auto PPU::scrolly_increment() -> void {
  if(raster_enable() == false) return;
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

auto PPU::raster_pixel() -> void {
  uint lx = hcounter() - 1;
  uint mask = 0x8000 >> (status.xaddr + (lx & 7));
  uint palette = 0, object_palette = 0;
  bool object_priority = 0;
  palette |= (raster.tiledatalo & mask) ? 1 : 0;
  palette |= (raster.tiledatahi & mask) ? 2 : 0;
  if(palette) {
    uint attr = raster.attribute;
    if(mask >= 256) attr >>= 2;
    palette |= (attr & 3) << 2;
  }

  if(status.bg_enable == false) palette = 0;
  if(status.bg_edge_enable == false && lx < 8) palette = 0;

  if(status.sprite_enable == true)
  for(int sprite = 7; sprite >= 0; sprite--) {
    if(status.sprite_edge_enable == false && lx < 8) continue;
    if(raster.oam[sprite].id == 64) continue;

    uint spritex = lx - raster.oam[sprite].x;
    if(spritex >= 8) continue;

    if(raster.oam[sprite].attr & 0x40) spritex ^= 7;
    uint mask = 0x80 >> spritex;
    uint sprite_palette = 0;
    sprite_palette |= (raster.oam[sprite].tiledatalo & mask) ? 1 : 0;
    sprite_palette |= (raster.oam[sprite].tiledatahi & mask) ? 2 : 0;
    if(sprite_palette == 0) continue;

    if(raster.oam[sprite].id == 0 && palette && lx != 255) status.sprite_zero_hit = 1;
    sprite_palette |= (raster.oam[sprite].attr & 3) << 2;

    object_priority = raster.oam[sprite].attr & 0x20;
    object_palette = 16 + sprite_palette;
  }

  if(object_palette) {
    if(palette == 0 || object_priority == 0) palette = object_palette;
  }

  if(raster_enable() == false) {
    if((status.vaddr & 0x3f00) != 0x3f00) palette = 0;
    else palette = status.vaddr;
  }
  output[vcounter() * 256 + lx] = (status.emphasis << 6) | cgramRead(palette);
}

auto PPU::raster_sprite() -> void {
  if(raster_enable() == false) return;

  uint n = raster.oam_iterator++;
  uint lastScanline = system.region() == System::Region::NTSC ? 261 : 311;
  int ly = (vcounter() == lastScanline ? (int)-1U : (int)vcounter());
  uint y = ly - oam[(n * 4) + 0];

  if(y >= sprite_height()) return;
  if(raster.oam_counter == 8) {
    status.sprite_overflow = 1;
    return;
  }

  raster.soam[raster.oam_counter].id   = n;
  raster.soam[raster.oam_counter].y    = oam[(n * 4) + 0];
  raster.soam[raster.oam_counter].tile = oam[(n * 4) + 1];
  raster.soam[raster.oam_counter].attr = oam[(n * 4) + 2];
  raster.soam[raster.oam_counter].x    = oam[(n * 4) + 3];
  raster.oam_counter++;
}

auto PPU::raster_scanline() -> void {
  uint last_scanline = system.region() == System::Region::NTSC ? 261 : 311;
  if((vcounter() >= 240 && vcounter() < last_scanline)) {
    addClocks(341);
    return scanline();
  }

  raster.oam_iterator = 0;
  raster.oam_counter = 0;

  for(uint n = 0; n < 8; n++) {
    raster.soam[n].id   = 64;
    raster.soam[n].y    = 0xff;
    raster.soam[n].tile = 0xff;
    raster.soam[n].attr = 0xff;
    raster.soam[n].x    = 0xff;
    raster.soam[n].tiledatalo = 0;
    raster.soam[n].tiledatahi = 0;
  }

  //  0
  addClocks(1);

  for(uint tile = 0; tile < 32; tile++) {  //  1-256
    uint nametable = chr_load(0x2000 | (status.vaddr & 0x0fff));
    uint tileaddr = status.bg_addr + (nametable << 4) + (scrolly() & 7);
    raster_pixel();
    addClocks(1);

    raster_pixel();
    addClocks(1);

    uint attribute = chr_load(0x23c0 | (status.vaddr & 0x0fc0) | ((scrolly() >> 5) << 3) | (scrollx() >> 5));
    if(scrolly() & 16) attribute >>= 4;
    if(scrollx() & 16) attribute >>= 2;
    raster_pixel();
    addClocks(1);

    scrollx_increment();
    if(tile == 31) scrolly_increment();
    raster_pixel();
    raster_sprite();
    addClocks(1);

    uint tiledatalo = chr_load(tileaddr + 0);
    raster_pixel();
    addClocks(1);

    raster_pixel();
    addClocks(1);

    uint tiledatahi = chr_load(tileaddr + 8);
    raster_pixel();
    addClocks(1);

    raster_pixel();
    raster_sprite();
    addClocks(1);

    raster.nametable = (raster.nametable << 8) | nametable;
    raster.attribute = (raster.attribute << 2) | (attribute & 3);
    raster.tiledatalo = (raster.tiledatalo << 8) | tiledatalo;
    raster.tiledatahi = (raster.tiledatahi << 8) | tiledatahi;
  }

  for(uint n = 0; n < 8; n++) raster.oam[n] = raster.soam[n];

  for(uint sprite = 0; sprite < 8; sprite++) {  //257-320
    uint nametable = chr_load(0x2000 | (status.vaddr & 0x0fff));
    addClocks(1);

    if(raster_enable() && sprite == 0) status.vaddr = (status.vaddr & 0x7be0) | (status.taddr & 0x041f);  //257
    addClocks(1);

    uint attribute = chr_load(0x23c0 | (status.vaddr & 0x0fc0) | ((scrolly() >> 5) << 3) | (scrollx() >> 5));
    uint tileaddr = (sprite_height() == 8)
    ? status.sprite_addr + raster.oam[sprite].tile * 16
    : ((raster.oam[sprite].tile & ~1) * 16) + ((raster.oam[sprite].tile & 1) * 0x1000);
    addClocks(2);

    uint spritey = (vcounter() - raster.oam[sprite].y) & (sprite_height() - 1);
    if(raster.oam[sprite].attr & 0x80) spritey ^= (sprite_height() - 1);
    tileaddr += spritey + (spritey & 8);

    raster.oam[sprite].tiledatalo = chr_load(tileaddr + 0);
    addClocks(2);

    raster.oam[sprite].tiledatahi = chr_load(tileaddr + 8);
    addClocks(2);

    if(raster_enable() && sprite == 6 && vcounter() == last_scanline) status.vaddr = status.taddr;  //304
  }

  for(uint tile = 0; tile < 2; tile++) {  //321-336
    uint nametable = chr_load(0x2000 | (status.vaddr & 0x0fff));
    uint tileaddr = status.bg_addr + (nametable << 4) + (scrolly() & 7);
    addClocks(2);

    uint attribute = chr_load(0x23c0 | (status.vaddr & 0x0fc0) | ((scrolly() >> 5) << 3) | (scrollx() >> 5));
    if(scrolly() & 16) attribute >>= 4;
    if(scrollx() & 16) attribute >>= 2;
    addClocks(1);

    scrollx_increment();
    addClocks(1);

    uint tiledatalo = chr_load(tileaddr + 0);
    addClocks(2);

    uint tiledatahi = chr_load(tileaddr + 8);
    addClocks(2);

    raster.nametable = (raster.nametable << 8) | nametable;
    raster.attribute = (raster.attribute << 2) | (attribute & 3);
    raster.tiledatalo = (raster.tiledatalo << 8) | tiledatalo;
    raster.tiledatahi = (raster.tiledatahi << 8) | tiledatahi;
  }

  //337-340
  chr_load(0x2000 | (status.vaddr & 0x0fff));
  addClocks(2);

  chr_load(0x2000 | (status.vaddr & 0x0fff));
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

auto PPU::exportRegisters(string &markup) -> void {
  markup.append("ppu\n");
  markup.append("  vaddr:       0x", hex(status.vaddr, 4L), "\n");
  markup.append("  taddr:       0x", hex(status.taddr, 4L), "\n");
  markup.append("  xaddr:       0x", hex(status.xaddr, 1L), "\n");
  markup.append("  sprite-size: ",   status.sprite_size ? "8×16" : "8×8", "\n");
  markup.append("  bg-addr:     0x", hex(status.bg_addr, 4L), "\n");
  markup.append("  sprite-addr: 0x", hex(status.sprite_addr, 4L), "\n");
}

}
