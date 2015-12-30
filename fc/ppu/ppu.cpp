#include <fc/fc.hpp>

namespace Famicom {

PPU ppu;

#include "serialization.cpp"

PPU::PPU() {
  surface = new uint32[256 * 312];
  output = surface + 0 * 256;
}

PPU::~PPU() {
  delete[] surface;
}

auto PPU::step(uint clocks) -> void {
  clock += clocks;
}

auto PPU::synchronizeCPU() -> void {
  if(clock >= 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(cpu.thread);
}

auto PPU::Enter() -> void { ppu.enter(); }

auto PPU::enter() -> void {
  while(true) {
    if(scheduler.sync == Scheduler::SynchronizeMode::PPU) {
      scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
    }

    raster_scanline();
  }
}

auto PPU::add_clocks(uint clocks) -> void {
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

auto PPU::scanline() -> void {
  if(vcounter() == 0) frame();
  cartridge.scanline(vcounter());
}

auto PPU::frame() -> void {
  system.frame();
  scheduler.exit(Scheduler::ExitReason::FrameEvent);
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
  memset(surface, 0, 256 * 312 * sizeof(uint32));

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

auto PPU::read(uint16 addr) -> uint8 {
  uint8 result = 0x00;

  switch(addr & 7) {
  case 2:  //PPUSTATUS
    result |= status.nmi_flag << 7;
    result |= status.sprite_zero_hit << 6;
    switch(revision) {
    default:
      result |= status.sprite_overflow << 5;
      result |= status.mdr & 0x1f;
      break;
    case Revision::RC2C05_01:
    case Revision::RC2C05_04: result |= 0x1b; break;
    case Revision::RC2C05_02: result |= 0x3d; break;
    case Revision::RC2C05_03: result |= 0x1c; break;
    }
    status.address_latch = 0;
    status.nmi_hold = 0;
    cpu.set_nmi_line(status.nmi_flag = 0);
    break;
  case 4:  //OAMDATA
    switch(revision) {
    case Revision::RP2C02C:
      result = status.mdr;
      break;
    default:
      result = oam_read(status.oam_addr);
      if((status.oam_addr & 3) == 2) {
        status.mdr = result;
        for(uint i = 0; i < 8; i++) status.mdr_decay[i] = 3221591;
      }
      break;
    }
    break;
  case 7:  //PPUDATA
    if(raster_enable() && (vcounter() <= 240 || vcounter() == (system.region() == System::Region::NTSC ? 261 : 311))) {
      return 0x00;
    }

    addr = status.vaddr & 0x3fff;
    if(addr <= 0x3eff) {
      result = status.bus_data;
    } else if(addr <= 0x3fff) {
      result = ((status.mdr & 0xc0) | cgram_read(addr));
    }
    status.bus_data = cartridge.chr_read(status.chr_abus = addr);
    status.vaddr += status.vram_increment;
    status.chr_abus = status.vaddr;
    status.mdr = result;
    break;
  default:
    result = status.mdr;
    break;
  }

  return result;
}

auto PPU::write(uint16 addr, uint8 data) -> void {
  status.mdr = data;
  // Decay rate can vary depending on the system and temperature.
  // Value used here is PPU's NTSC clock rate * 0.6 rounded to nearest integer.
  for(uint i = 0; i < 8; i++) status.mdr_decay[i] = 3221591;

  switch(revision) {
  case Revision::RC2C05_01:
  case Revision::RC2C05_02:
  case Revision::RC2C05_03:
  case Revision::RC2C05_04:
  case Revision::RC2C05_05: if((addr & 6) == 0) addr ^= 1; break;
  }

  switch(addr & 7) {
  case 0:  //PPUCTRL
    status.nmi_enable = data & 0x80;
    status.master_select = data & 0x40;
    status.sprite_size = data & 0x20;
    status.bg_addr = (data & 0x10) ? 0x1000 : 0x0000;
    status.sprite_addr = (data & 0x08) ? 0x1000 : 0x0000;
    status.vram_increment = (data & 0x04) ? 32 : 1;
    status.taddr = (status.taddr & 0x73ff) | ((data & 0x03) << 10);
    cpu.set_nmi_line(status.nmi_enable && status.nmi_hold && status.nmi_flag);
    return;
  case 1:  //PPUMASK
    status.emphasis = data >> 5;
    if(revision == Revision::RP2C07) {
      status.emphasis =
        ((status.emphasis & 1) << 1) | // swap red
        ((status.emphasis & 2) >> 1) | // and green
        ((status.emphasis & 4)     );
    }
    status.sprite_enable = data & 0x10;
    status.bg_enable = data & 0x08;
    status.sprite_edge_enable = data & 0x04;
    status.bg_edge_enable = data & 0x02;
    status.grayscale = data & 0x01;
    return;
  case 2:  //PPUSTATUS
    return;
  case 3:  //OAMADDR
    if(revision != Revision::RP2C07) {
      // below corruption code only applies for preferred CPU-PPU alignment.
      // on an actual Famicom/NES, waiting a while after writing to OAM will
      // make this corruption happen because the OAM will have decayed at the
      // spot being written to.
      for(int i = 0; i < 8; i++)
        oam[((addr & 0xf800) >> 8) + i] = oam[(status.oam_addr & 0xf8) + i];
    }
    status.oam_addr = data;
    return;
  case 4:  //OAMDATA
    oam_write(status.oam_addr++, data);
    return;
  case 5:  //PPUSCROLL
    if(status.address_latch == 0) {
      status.xaddr = data & 0x07;
      status.taddr = (status.taddr & 0x7fe0) | (data >> 3);
    } else {
      status.taddr = (status.taddr & 0x0c1f) | ((data & 0x07) << 12) | ((data >> 3) << 5);
    }
    status.address_latch ^= 1;
    return;
  case 6:  //PPUADDR
    if(status.address_latch == 0) {
      status.taddr = (status.taddr & 0x00ff) | ((data & 0x3f) << 8);
    } else {
      status.taddr = (status.taddr & 0x7f00) | data;
      status.vaddr = status.taddr;
      status.chr_abus = status.vaddr;
    }
    status.address_latch ^= 1;
    return;
  case 7:  //PPUDATA
    if(raster_enable() && (vcounter() <= 240 || vcounter() == (system.region() != System::Region::PAL ? 261 : 311))) {
      return;
    }

    addr = status.vaddr & 0x3fff;
    if(addr <= 0x3eff) {
      cartridge.chr_write(status.chr_abus = addr, data);
    } else if(addr <= 0x3fff) {
      cgram_write(addr, data);
    }
    status.vaddr += status.vram_increment;
    status.chr_abus = status.vaddr;
    return;
  }
}

auto PPU::ciram_read(uint14 addr) -> uint8 {
  return ciram[addr & (!system.vs() ? 0x07ff : 0x0fff)];
}

auto PPU::ciram_write(uint14 addr, uint8 data) -> void {
  ciram[addr & (!system.vs() ? 0x07ff : 0x0fff)] = data;
}

auto PPU::cgram_read(uint14 addr) -> uint8 {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  uint8 data = cgram[addr & 0x1f];
  if(status.grayscale) data &= 0x30;
  return data;
}

auto PPU::cgram_write(uint14 addr, uint8 data) -> void {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  cgram[addr & 0x1f] = data;
}

auto PPU::oam_read(uint8 addr) -> uint8 {
  uint8 data = oam[addr];
  return data;
}

auto PPU::oam_write(uint8 addr, uint8 data) -> void {
  if((addr & 3) == 2) data &= 0xe3;
  oam[addr] = data;
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
  output[vcounter() * 256 + lx] = (status.emphasis << 6) | cgram_read(palette);
}

auto PPU::raster_sprite() -> void {
  if(raster_enable() == false) return;

  uint n = raster.oam_iterator++;
  int ly = (vcounter() == (system.region() == System::Region::NTSC ? 261 : 311) ? -1 : vcounter());
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
    add_clocks(341);
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
  add_clocks(1);

  for(uint tile = 0; tile < 32; tile++) {  //  1-256
    uint nametable = chr_load(0x2000 | (status.vaddr & 0x0fff));
    uint tileaddr = status.bg_addr + (nametable << 4) + (scrolly() & 7);
    raster_pixel();
    add_clocks(1);

    raster_pixel();
    add_clocks(1);

    uint attribute = chr_load(0x23c0 | (status.vaddr & 0x0fc0) | ((scrolly() >> 5) << 3) | (scrollx() >> 5));
    if(scrolly() & 16) attribute >>= 4;
    if(scrollx() & 16) attribute >>= 2;
    raster_pixel();
    add_clocks(1);

    scrollx_increment();
    if(tile == 31) scrolly_increment();
    raster_pixel();
    raster_sprite();
    add_clocks(1);

    uint tiledatalo = chr_load(tileaddr + 0);
    raster_pixel();
    add_clocks(1);

    raster_pixel();
    add_clocks(1);

    uint tiledatahi = chr_load(tileaddr + 8);
    raster_pixel();
    add_clocks(1);

    raster_pixel();
    raster_sprite();
    add_clocks(1);

    raster.nametable = (raster.nametable << 8) | nametable;
    raster.attribute = (raster.attribute << 2) | (attribute & 3);
    raster.tiledatalo = (raster.tiledatalo << 8) | tiledatalo;
    raster.tiledatahi = (raster.tiledatahi << 8) | tiledatahi;
  }

  for(uint n = 0; n < 8; n++) raster.oam[n] = raster.soam[n];

  for(uint sprite = 0; sprite < 8; sprite++) {  //257-320
    uint nametable = chr_load(0x2000 | (status.vaddr & 0x0fff));
    add_clocks(1);

    if(raster_enable() && sprite == 0) status.vaddr = (status.vaddr & 0x7be0) | (status.taddr & 0x041f);  //257
    add_clocks(1);

    uint attribute = chr_load(0x23c0 | (status.vaddr & 0x0fc0) | ((scrolly() >> 5) << 3) | (scrollx() >> 5));
    uint tileaddr = (sprite_height() == 8)
    ? status.sprite_addr + raster.oam[sprite].tile * 16
    : ((raster.oam[sprite].tile & ~1) * 16) + ((raster.oam[sprite].tile & 1) * 0x1000);
    add_clocks(2);

    uint spritey = (vcounter() - raster.oam[sprite].y) & (sprite_height() - 1);
    if(raster.oam[sprite].attr & 0x80) spritey ^= (sprite_height() - 1);
    tileaddr += spritey + (spritey & 8);

    raster.oam[sprite].tiledatalo = chr_load(tileaddr + 0);
    add_clocks(2);

    raster.oam[sprite].tiledatahi = chr_load(tileaddr + 8);
    add_clocks(2);

    if(raster_enable() && sprite == 6 && vcounter() == last_scanline) status.vaddr = status.taddr;  //304
  }

  for(uint tile = 0; tile < 2; tile++) {  //321-336
    uint nametable = chr_load(0x2000 | (status.vaddr & 0x0fff));
    uint tileaddr = status.bg_addr + (nametable << 4) + (scrolly() & 7);
    add_clocks(2);

    uint attribute = chr_load(0x23c0 | (status.vaddr & 0x0fc0) | ((scrolly() >> 5) << 3) | (scrollx() >> 5));
    if(scrolly() & 16) attribute >>= 4;
    if(scrollx() & 16) attribute >>= 2;
    add_clocks(1);

    scrollx_increment();
    add_clocks(1);

    uint tiledatalo = chr_load(tileaddr + 0);
    add_clocks(2);

    uint tiledatahi = chr_load(tileaddr + 8);
    add_clocks(2);

    raster.nametable = (raster.nametable << 8) | nametable;
    raster.attribute = (raster.attribute << 2) | (attribute & 3);
    raster.tiledatalo = (raster.tiledatalo << 8) | tiledatalo;
    raster.tiledatahi = (raster.tiledatahi << 8) | tiledatahi;
  }

  //337-340
  chr_load(0x2000 | (status.vaddr & 0x0fff));
  add_clocks(2);

  chr_load(0x2000 | (status.vaddr & 0x0fff));
  add_clocks(1);
  if(hcounter() > 0) add_clocks(1);

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
