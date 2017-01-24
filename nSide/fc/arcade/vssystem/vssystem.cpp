#include <fc/fc.hpp>

namespace Famicom {

VSSystem vssystem;

#include "control.cpp"
#include "serialization.cpp"

VSSystem::VSSystem() {
  forceSubRAM = false;
  gameCount = GameCount::Uni;
}

auto VSSystem::Enter() -> void {
  while(true) scheduler.synchronize(), vssystem.main();
}

auto VSSystem::main() -> void {
  //if(++watchdog > system.cpuFrequency() * 4 / 3) {
  //  watchdog = 0;
  //  if(gameCount == GameCount::Dual) cpu0.reset();
  //  cpu1.reset();
  //}
  step(1);
  if(gameCount == GameCount::Dual) synchronize(cpu0);
  synchronize(cpu1);
}

auto VSSystem::load() -> bool {
  return true;
}

auto VSSystem::power() -> void {
  create(VSSystem::Enter, system.colorburst() * 6.0);

  function<auto (uint16, uint8) -> uint8> reader;
  function<auto (uint16, uint8) -> void> writer;

  reader = [](uint16 addr, uint8 data) -> uint8 { return vssystem.read(0, addr, data); };
  writer = [](uint16 addr, uint8 data) -> void { vssystem.write(0, addr, data); };
  bus0.map(reader, writer, "4016-4017");
  bus0.map(reader, writer, "4020-5fff", 0, 0, 0x0020);
  bus0.map(reader, writer, "6000-7fff");

  reader = [](uint16 addr, uint8 data) -> uint8 { return vssystem.read(1, addr, data); };
  writer = [](uint16 addr, uint8 data) -> void { vssystem.write(1, addr, data); };
  bus1.map(reader, writer, "4016-4017");
  bus1.map(reader, writer, "4020-5fff", 0, 0, 0x0020);
  bus1.map(reader, writer, "6000-7fff");

  ramSide = forceSubRAM ? 1 : 0;
  resetButtons();
}

auto VSSystem::setDip(bool side, uint8 dip) -> void {
  if(side == 0) mainDip = dip;
  if(side == 1) subDip = dip;
}

auto VSSystem::read(bool side, uint16 addr, uint8 data) -> uint8 {
  if(addr == 0x4016) {
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

  if(addr == 0x4017) {
    if(side == 1) watchdog = 0;
    data = 0x00;
    data |= data2(side); // buttons 2 and 4
    uint& counter = side == 0 ? mainControlCounter2 : subControlCounter2;
    counter++;
    if(side == 0) data |= mainDip & 0xfc;
    if(side == 1) data |= subDip & 0xfc;
    return data;
  }

  if(addr >= 0x4020 && addr <= 0x5fff) {
    write(side, addr, data);
    return data;
  }

  if(addr >= 0x6000 && addr <= 0x7fff) {
    return side == ramSide ? ram[addr & 0x07ff] : data;
  }
}

auto VSSystem::write(bool side, uint16 addr, uint8 data) -> void {
  if(addr == 0x4016) {
    (side ? cpu1 : cpu0).writeCPU(addr, data);
    latch(side, data.bit(0));
    if(side == 0 && !forceSubRAM) ramSide = !data.bit(1);
  }

  if(addr >= 0x4020 && addr <= 0x5fff) {
    //increment coin counter
  }

  if(addr >= 0x6000 && addr <= 0x7fff) {
    if(side == ramSide) ram[addr & 0x07ff] = data;
  }
}

}
