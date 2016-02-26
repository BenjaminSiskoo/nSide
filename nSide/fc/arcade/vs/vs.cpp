#include <fc/fc.hpp>

namespace Famicom {

VSArcadeBoard vsarcadeboard;

#include "control.cpp"
#include "serialization.cpp"

auto VSArcadeBoard::Enter() -> void {
  while(true) scheduler.synchronize(), vsarcadeboard.main();
}

auto VSArcadeBoard::main() -> void {
  if(++watchdog > system.cpuFrequency() * 4 / 3) {
    watchdog = 0;
    cpu.reset();
    //ppu.reset();
  }
  step(1);
}

auto VSArcadeBoard::step(uint clocks) -> void {
  clock += clocks;
  synchronizeCPU();
}

auto VSArcadeBoard::synchronizeCPU() -> void {
  if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
}

auto VSArcadeBoard::init() -> void {
  forceSubRam = false;
}

auto VSArcadeBoard::load() -> void {
}

auto VSArcadeBoard::unload() -> void {
}

auto VSArcadeBoard::power() -> void {
  reset();
}

auto VSArcadeBoard::reset() -> void {
  create(VSArcadeBoard::Enter, system.cpuFrequency());
  ramSide = forceSubRam ? 1 : 0;
  resetButtons();
}

auto VSArcadeBoard::setDip(bool side, uint8 dip) -> void {
  if(side == 0) mainDip = dip;
  if(side == 1) subDip = dip;
}

auto VSArcadeBoard::read(bool side, uint16 addr, uint8 data) -> uint8 {
  if(addr == 0x4016) return r4016(side, data);
  if(addr == 0x4017) return r4017(side, data);
  if(addr >= 0x4020 && addr <= 0x5fff) return r4020(side, data);
  if(addr >= 0x6000 && addr <= 0x7fff) {
    return side == ramSide ? ram[addr & 0x07ff] : data;
  }
}

auto VSArcadeBoard::write(bool side, uint16 addr, uint8 data) -> void {
  if(addr == 0x4016) w4016(side, data);
  if(addr >= 0x4020 && addr <= 0x5fff) w4020(side, data);
  if(addr >= 0x6000 && addr <= 0x7fff) {
    if(side == ramSide) ram[addr & 0x07ff] = data;
  }
}

auto VSArcadeBoard::r4016(bool side, uint8 data) -> uint8 {
  data = (!side) << 7; // 0x00 for slave CPU, 0x80 for master CPU
  data |= data1(side);
  uint& counter = side == 0 ? mainControlCounter1 : subControlCounter1;
  counter++;
  if(side == 0) data |= (mainDip & 0x03) << 3;
  if(side == 1) data |= (subDip & 0x03) << 3;
  data |= (
    poll(side, ServiceButton) << 2 |
    poll(side, Coin1) << 5 |
    poll(side, Coin2) << 6
  );
  return data;
}

auto VSArcadeBoard::r4017(bool side, uint8 data) -> uint8 {
  //if(side == 1) watchdog = 0;
  data = 0x00;
  data |= data2(side); // buttons 2 and 4
  uint& counter = side == 0 ? mainControlCounter2 : subControlCounter2;
  counter++;
  if(side == 0) data |= mainDip & 0xfc;
  if(side == 1) data |= subDip & 0xfc;
  return data;
}

auto VSArcadeBoard::r4020(bool side, uint8 data) -> uint8 {
  write(side, 0x4020, data);
  return data;
}

auto VSArcadeBoard::w4016(bool side, uint8 data) -> void {
  device.controllerPort1->latch(data & 1);
  device.controllerPort2->latch(data & 1);
  device.expansionPort->latch(data & 1);
  latch(side, data & 1);
  if(side == 0 && !forceSubRam) ramSide = data & 2 ^ 2;
}

auto VSArcadeBoard::w4020(bool side, uint8 data) -> void {
  // increment coin counter
}

}
