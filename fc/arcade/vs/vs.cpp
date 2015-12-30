#include <fc/fc.hpp>

namespace Famicom {

VSArcadeBoard vsarcadeboard;

#include "serialization.cpp"

auto VSArcadeBoard::init() -> void {
}

auto VSArcadeBoard::load() -> void {
}

auto VSArcadeBoard::unload() -> void {
}

auto VSArcadeBoard::power() -> void {
}

auto VSArcadeBoard::reset() -> void {
}

auto VSArcadeBoard::set_dip(uint16 dip) -> void {
  this->dip = dip;
}

auto VSArcadeBoard::read(uint16 addr) -> uint8 {
  if(addr == 0x4016) return r4016();
  if(addr == 0x4017) return r4017();
  if((addr & 0xe020) == 0x4020) return r4020();
  if(addr >= 0x6000 && addr <= 0x7fff) return ram[addr & 0x07ff];
}

auto VSArcadeBoard::write(uint16 addr, uint8 data) -> void {
  if(addr == 0x4016) w4016(data);
  if((addr & 0xe020) == 0x4020) w4020(data);
  if(addr >= 0x6000 && addr <= 0x7fff) ram[addr & 0x07ff] = data;
}

auto VSArcadeBoard::r4016() -> uint8 {
  uint8 data = 0x80; // 0x00 for slave CPU, 0x80 for master CPU
  if(!swap_controllers) data |= device.controllerPort1->data() & 0x03;
  else                  data |= device.controllerPort2->data() & 0x03;
  data |= device.expansionPort->data1(); // buttons 1 and 3
  data |= (dip & 0x03) << 3;
  data |= device.expansionPort->data() << 2; // Service button and coins
  return data;
}

auto VSArcadeBoard::r4017() -> uint8 {
  uint8 data = 0x00;
  if(!swap_controllers) data |= device.controllerPort2->data() & 0x03;
  else                  data |= device.controllerPort1->data() & 0x03;
  data |= device.expansionPort->data2(); // buttons 2 and 4
  data |= dip & 0xfc;
  return data;
}

auto VSArcadeBoard::r4020() -> uint8 {
  write(0x4020, cpu.mdr());
  return cpu.mdr();
}

auto VSArcadeBoard::w4016(uint8 data) -> void {
  device.controllerPort1->latch(data & 1);
  device.controllerPort2->latch(data & 1);
  device.expansionPort->latch(data & 1);
}

auto VSArcadeBoard::w4020(uint8 data) -> void {
  // increment coin counter
}

}
