#include <fc/fc.hpp>

namespace Famicom {

PC10ArcadeBoard pc10arcadeboard;

#include "video-circuit.cpp"
#include "serialization.cpp"

auto PC10ArcadeBoard::init() -> void {
  dip = 0;
}

auto PC10ArcadeBoard::load() -> void {
}

auto PC10ArcadeBoard::unload() -> void {
}

auto PC10ArcadeBoard::power() -> void {
  vram_access = 1; // 0: Z80,                  1: video circuit
  controls    = 1; // 0: disable START/SELECT, 1: enable START/SELECT
  ppu_output  = 1; // 0: disable,              1: enable
  apu_output  = 1; // 0: disable,              1: enable
  cpu_reset   = 1; // 0: reset,                1: run
  cpu_stop    = 1; // 0: stop,                 1: run
  display     = 1; // 0: video circuit,        1: PPU
  z80_nmi     = 0; // 0: disable,              1: enable
  watchdog    = 1; // 0: enable,               1: disable
  ppu_reset   = 1; // 0: reset,                1: run
  channel     = 0; // 0-9 internally, 1-10 to players
  sram_bank   = 1; // bank at $8c00-8fff
  video_power();
  update_video();
}

auto PC10ArcadeBoard::reset() -> void {
}

auto PC10ArcadeBoard::set_dip(uint16 dip) -> void {
  this->dip = dip;
}

auto PC10ArcadeBoard::read(uint16 addr) -> uint8 {
  if(addr <= 0x7fff) return bios[addr & 0x3fff];
  if(addr <= 0x87ff) return wram[addr & 0x07ff];
  if(addr <= 0x8bff) return sram[addr & 0x03ff];
  if(addr <= 0x8fff) return sram[(addr & 0x03ff) | (sram_bank << 10)];
  if(addr <= 0x97ff) return 0x00; // VRAM is write-only
  if(addr <= 0xbfff) return 0x00;
  if(addr <= 0xdfff) return cartridge.board->instrom.read(addr & 0x1fff);
  if(addr <= 0xffff) {
    uint8 data = 0xe7;
    if(!prom_test || prom_address < 0x40)
      data |= (((cartridge.board->keyrom.read(prom_address >> 3) >> (prom_address & 7)) & 1) << 3) ^ 0x08;
    else
      data |= (prom_address & 2 ? (uint8)0x00 : cartridge.board->keyrom.read(8)) ^ 0x08;
    data |= ((prom_address & 0x20) >> 1) ^ 0x10;
    return data;
  };
}

auto PC10ArcadeBoard::write(uint16 addr, uint8 data) -> void {
       if(addr <= 0x7fff) return;
  else if(addr <= 0x87ff) wram[addr & 0x07ff] = data;
  else if(addr <= 0x8bff) sram[addr & 0x03ff] = data;
  else if(addr <= 0x8fff) sram[(addr & 0x03ff) | (sram_bank << 10)] = data;
  else if(addr <= 0x97ff) vram[addr & 0x07ff] = data;
  else if(addr <= 0xbfff) return;
  else if(addr <= 0xdfff) return;
  else if(addr <= 0xffff) {
    prom_test = data & 0x10;
    if(prom_clock && (data & 0x08) == 0) prom_address++;
    prom_clock = data & 0x08;
    if((data & 0x01) == 0) prom_address = 0;
    return;
  }
}

auto PC10ArcadeBoard::io_read(uint16 addr) -> uint8 {
  uint8 data = 0x00;
  bool channel_select = false;
  bool enter          = false;
  bool reset          = false;
  bool coin_2         = false;
  bool service        = false;
  bool coin_1         = false;
  switch(addr & 0x0003) {
  case 0x0000:
    data |= channel_select      << 0;
    data |= enter               << 1;
    data |= reset               << 2;
    data |= cpu.status.nmi_line << 3;
    data |= 0                   << 4;
    data |= coin_2              << 5;
    data |= service             << 6;
    data |= coin_1              << 7;
    break;
  case 0x0001: data = (dip >> 0) & 0xff; break;
  case 0x0002: data = (dip >> 8) & 0xff; break;
  case 0x0003: data = 0x00; break;
  }
  return data;
}

auto PC10ArcadeBoard::io_write(uint16 addr, uint8 data) -> void {
  data &= 0x01;
  switch(addr & 0x001f) {
  case 0x0000: vram_access = data; break;
  case 0x0001: controls    = data; break;
  case 0x0002: ppu_output  = data; break;
  case 0x0003: apu_output  = data; break;
  case 0x0004: cpu_reset   = data; break;
  case 0x0005: cpu_stop    = data; break;
  case 0x0006: display     = data; break;
  case 0x0008: z80_nmi     = data; break;
  case 0x0009: watchdog    = data; break;
  case 0x000a: ppu_reset   = data; break;
  case 0x000b: channel = (channel & 0xe) | (data << 0); break;
  case 0x000c: channel = (channel & 0xd) | (data << 1); break;
  case 0x000d: channel = (channel & 0xb) | (data << 2); break;
  case 0x000e: channel = (channel & 0x7) | (data << 3); break;
  case 0x000f: sram_bank   = data; break;
  }
  switch(addr & 0x13) {
  case 0x0010: break;
  case 0x0011: break;
  case 0x0012: break;
  case 0x0013: break;
  }
}

}
