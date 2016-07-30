#include <fc/fc.hpp>

namespace Famicom {

PlayChoice10 playchoice10;
#include "cpu.cpp"
#include "video-circuit.cpp"
#include "serialization.cpp"

auto PlayChoice10::init() -> void {
  dip = 0;
}

auto PlayChoice10::load() -> void {
}

auto PlayChoice10::unload() -> void {
}

auto PlayChoice10::power() -> void {
  pc10cpu.power();
  vramAccess = 1; // 0: Z80,                  1: video circuit
  controls   = 1; // 0: disable START/SELECT, 1: enable START/SELECT
  ppuOutput  = 1; // 0: disable,              1: enable
  apuOutput  = 1; // 0: disable,              1: enable
  cpuReset   = 1; // 0: reset,                1: run
  cpuStop    = 1; // 0: stop,                 1: run
  display    = 1; // 0: video circuit,        1: PPU
  z80NMI     = 0; // 0: disable,              1: enable
  watchdog   = 1; // 0: enable,               1: disable
  ppuReset   = 1; // 0: reset,                1: run
  channel    = 0; // 0-9 internally, 1-10 on screen
  sramBank   = 1; // bank at $8c00-8fff
  videoCircuit.power();
  videoCircuit.update();
}

auto PlayChoice10::reset() -> void {
  pc10cpu.reset();
}

auto PlayChoice10::setDip(uint16 dip) -> void {
  this->dip = dip;
}

auto PlayChoice10::read(uint16 addr) -> uint8 {
  if(addr <= 0x7fff) return bios[addr & 0x3fff];
  if(addr <= 0x87ff) return wram[addr & 0x07ff];
  if(addr <= 0x8bff) return sram[addr & 0x03ff];
  if(addr <= 0x8fff) return sram[(addr & 0x03ff) | (sramBank << 10)];
  if(addr <= 0x97ff) return 0x00; // VRAM is write-only
  if(addr <= 0xbfff) return 0x00;
  if(addr <= 0xdfff) return cartridge.board->instrom.read(addr & 0x1fff);
  if(addr <= 0xffff) {
    uint8 data = 0xe7;
    if(!promTest || promAddress < 0x40)
      data |= (((cartridge.board->keyrom.read(promAddress >> 3) >> (promAddress & 7)) & 1) << 3) ^ 0x08;
    else
      data |= (promAddress & 2 ? (uint8)0x00 : cartridge.board->keyrom.read(8)) ^ 0x08;
    data |= ((promAddress & 0x20) >> 1) ^ 0x10;
    return data;
  };
}

auto PlayChoice10::write(uint16 addr, uint8 data) -> void {
       if(addr <= 0x7fff) return;
  else if(addr <= 0x87ff) wram[addr & 0x07ff] = data;
  else if(addr <= 0x8bff) sram[addr & 0x03ff] = data;
  else if(addr <= 0x8fff) sram[(addr & 0x03ff) | (sramBank << 10)] = data;
  else if(addr <= 0x97ff) videoCircuit.vram[addr & 0x07ff] = data;
  else if(addr <= 0xbfff) return;
  else if(addr <= 0xdfff) return;
  else if(addr <= 0xffff) {
    promTest = data & 0x10;
    if(promClock && (data & 0x08) == 0) promAddress++;
    promClock = data & 0x08;
    if((data & 0x01) == 0) promAddress = 0;
    return;
  }
}

auto PlayChoice10::portRead(uint8 port) -> uint8 {
  uint8 data = 0x00;
  bool channelSelect = false;
  bool enter         = false;
  bool reset         = false;
  bool coin2         = false;
  bool service       = false;
  bool coin1         = false;
  switch(port & 0x03) {
  case 0x00:
    data |= channelSelect      << 0;
    data |= enter              << 1;
    data |= reset              << 2;
    data |= cpu.status.nmiLine << 3;
    data |= 0                  << 4;
    data |= coin2              << 5;
    data |= service            << 6;
    data |= coin1              << 7;
    break;
  case 0x01: data = (dip >> 0) & 0xff; break;
  case 0x02: data = (dip >> 8) & 0xff; break;
  case 0x03: data = 0x00; break;
  }
  return data;
}

auto PlayChoice10::portWrite(uint8 port, uint8 data) -> void {
  data &= 0x01;
  switch(port & 0x1f) {
  case 0x00: vramAccess = data; break;
  case 0x01: controls   = data; break;
  case 0x02: ppuOutput  = data; break;
  case 0x03: apuOutput  = data; break;
  case 0x04: cpuReset   = data; break;
  case 0x05: cpuStop    = data; break;
  case 0x06: display    = data; break;
  case 0x08: z80NMI     = data; break;
  case 0x09: watchdog   = data; break;
  case 0x0a: ppuReset   = data; break;
  case 0x0b: channel    = (channel & 0xe) | (data << 0); break;
  case 0x0c: channel    = (channel & 0xd) | (data << 1); break;
  case 0x0d: channel    = (channel & 0xb) | (data << 2); break;
  case 0x0e: channel    = (channel & 0x7) | (data << 3); break;
  case 0x0f: sramBank   = data; break;
  }
  switch(port & 0x13) {
  case 0x10: break;
  case 0x11: break;
  case 0x12: break;
  case 0x13: break;
  }
}

}
