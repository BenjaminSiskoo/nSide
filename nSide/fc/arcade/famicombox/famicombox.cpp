#include <fc/fc.hpp>

namespace Famicom {

FamicomBox famicombox;

#include "serialization.cpp"

auto FamicomBox::Enter() -> void {
  while(true) scheduler.synchronize(), famicombox.main();
}

auto FamicomBox::main() -> void {
  if(--counter == 0) {
    if(--attractionTimer == 0x7fff) trap(Exception::AttractionTimer);
    //if(++watchdog        == 0x0000) trap(Exception::Watchdog);
    counter = 3 * 0x2000;
  }
  step(1);
  synchronize(cpu);
}

auto FamicomBox::init() -> void {
  dip = 0x3da;
}

auto FamicomBox::load(Markup::Node node) -> bool {
  if(auto bios_prg = node["prg/rom/name"].text()) {
    if(auto fp = interface->open(ID::System, bios_prg, File::Read, File::Required)) {
      fp->read(famicombox.bios_prg, 32768);
    } else return false;
  }

  if(auto bios_chr = node["chr/rom/name"].text()) {
    if(auto fp = interface->open(ID::System, bios_chr, File::Read, File::Required)) {
      fp->read(famicombox.bios_chr, 8192);
    } else return false;
  }

  return true;
}

auto FamicomBox::unload() -> void {
}

auto FamicomBox::power() -> void {
  exceptionEnable = 0x00;
}

auto FamicomBox::reset() -> void {
  create(FamicomBox::Enter, system.colorburst() * 6.0);

  function<auto (uint16, uint8) -> uint8> reader;
  function<auto (uint16, uint8) -> void> writer;

  reader = {&FamicomBox::wramRead, this};
  writer = {&FamicomBox::wramWrite, this};
  bus.map(reader, writer, "0800-1fff");

  reader = {&FamicomBox::portRead, this};
  writer = {&FamicomBox::portWrite, this};
  bus.map(reader, writer, "4016-4017");
  bus.map(reader, writer, "5000-5fff");

  reader = {&FamicomBox::sramRead, this};
  writer = {&FamicomBox::sramWrite, this};
  bus.map(reader, writer, "6000-7fff");

  reader = {&FamicomBox::cartridgeRead, this};
  writer = {&FamicomBox::cartridgeWrite, this};
  bus.map(reader, writer, "8000-ffff");

  // The cartridge is only mapped to $8000-ffff, not $4018-ffff.

  exceptionTrap = 0xff;
  ramProtect = 7;
  counter = 3 * 0x2000;
  attractionTimer.bits(14,7) = 0xff;
  watchdog.bits(13,10) = 0x00;

  cartridgeSelect = 1;
  cartridgeRowSelect = 1;
  registerLock = false;
}

auto FamicomBox::setDip(uint10 dip) -> void {
  this->dip = dip;
}

auto FamicomBox::trap(Exception exceptionId) -> void {
  if(exceptionId != Exception::ControllerRead) print("Trap Exception ", (uint)exceptionId, "\n");
  if(exceptionId != Exception::Watchdog
  && exceptionEnable.bit((int)exceptionId) == (exceptionId == Exception::Interrupt6_82Hz)) return;
  exceptionTrap.bit((int)exceptionId) = 0;
  cartridge.reset();
  cpu.reset();
  apu.reset();
}

auto FamicomBox::wramRead(uint16 addr, uint8 data) -> uint8 {
  if(addr >= 0x0800 && addr <= 0x1fff) print("read  $", hex(addr, 4U), "\n");
  switch(addr & 0xf800) {
  case 0x0000: data = cpu.ram[addr]; break;
  case 0x0800:
  case 0x1000:
  case 0x1800: data = bios_ram[addr - 0x800]; break;
  }
}

auto FamicomBox::portRead(uint16 addr, uint8 data) -> uint8 {
  if((addr & 0xf000) == 0x5000) print("read  $", hex(addr, 4U), "\n");

  if(addr == 0x4016 || addr == 0x4017) {
    data = cpu.readCPU(addr, data);
    if(data == 0x4017 && !dip.bit(9)) data.bits(4,3) = 0;
    watchdog.bits(13,10) = 0;
    trap(Exception::ControllerRead);
  }

  if(!registerLock) {
    switch(addr & 0xf007) {

    //Exception trap flags
    case 0x5000: {
      data = exceptionTrap;
      exceptionTrap = 0xff;
      return data;
    }

    //Not used
    case 0x5001: {
      return data;
    }

    //DIP Switches
    case 0x5002: {
      return dip & 0xff;
    }

    //Keyswitch position and coin module status
    case 0x5003: {
      return 0x08;
    }

    //Test connector (25-pin) inputs 2, 15, 3, 16, 4, 17, 5, 18
    case 0x5004: {
      return data;
    }

    //Expansion connector (50-pin) input pin 28
    case 0x5005: {
      return data;
    }

    //Expansion connector (50-pin) input pin 27
    case 0x5006: {
      return data;
    }

    //Misc. status
    case 0x5007: {
      return 0x00;
    }

    }
  }

  return data;
}

auto FamicomBox::sramRead(uint16 addr, uint8 data) -> uint8 {
  return test_ram[addr & 0x1fff];
}

auto FamicomBox::cartridgeRead(uint16 addr, uint8 data) -> uint8 {
  switch(cartridgeRowSelect) {
  case 0:
    if(cartridgeSelect == 0) return bios_prg[addr];
    break;
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.readPRG(addr);
    case  2: return data;
    case  3: return data;
    case  4: return data;
    case  5: return data;
    }
    break;
  case 2:
    switch(cartridgeSelect) {
    case  6: return data;
    case  7: return data;
    case  8: return data;
    case  9: return data;
    case 10: return data;
    }
    break;
  case 3:
    switch(cartridgeSelect) {
    case 11: return data;
    case 12: return data;
    case 13: return data;
    case 14: return data;
    case 15: return data;
    }
    break;
  }
  return data;
}

auto FamicomBox::portWrite(uint16 addr, uint8 data) -> void {
  if((addr & 0xf000) == 0x5000) print("write $", hex(addr, 4U), " 0x", hex(data, 2U), "\n");

  if(addr >= 0x4016 && addr <= 0x4017) return cpu.writeCPU(addr, data);

  if(!registerLock) {
    switch(addr & 0xf007) {

    //Exception enable flags
    case 0x5000: {
      exceptionEnable = data & 0xbf;
      break;
    }

    //Coin module flags and CATV output
    case 0x5001: {
      break;
    }

    //Cartridge slot LED and RAM protect register
    case 0x5002: {
      //ramProtect = data.bits(6,4);
      break;
    }

    //Attraction timer
    case 0x5003: {
      attractionTimer.bits(14,7) = 0x00;
      break;
    }

    //Cartridge selection register
    case 0x5004: {
      cartridgeSelect = data.bits(3,0);
      cartridgeRowSelect = data.bits(5,4);
      registerLock |= data.bit(6);
      break;
    }

    //Misc. control
    case 0x5005: {
      break;
    }

    //Test connector DB-25 outputs 6, 15, 7, 16, 8, 17, 9, 18
    case 0x5006: {
      break;
    }

    //Expansion connector (50-pin) output pin 26
    case 0x5007: {
      break;
    }

    }
  }
}

auto FamicomBox::wramWrite(uint16 addr, uint8 data) -> void {
  if(addr >= 0x0800 && addr <= 0x1fff) print("write $", hex(addr, 4U), " 0x", hex(data, 2U), "\n");
  switch(addr & 0xf800) {
  case 0x0000: {
    if(ramProtect >= 1) cpu.ram[addr] = data;
    return;
  }

  case 0x0800: {
    if(ramProtect >= 2) bios_ram[addr - 0x800] = data;
    return;
  }

  case 0x1000: {
    if(ramProtect >= 3) bios_ram[addr - 0x800] = data;
    return;
  }

  case 0x1800: {
    if(ramProtect >= 4) bios_ram[addr - 0x800] = data;
    return;
  }

  }
}

auto FamicomBox::sramWrite(uint16 addr, uint8 data) -> void {
  test_ram[addr & 0x1fff] = data;
}

auto FamicomBox::cartridgeWrite(uint16 addr, uint8 data) -> void {
  switch(cartridgeRowSelect) {
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.writePRG(addr, data);
    case  2: return;
    case  3: return;
    case  4: return;
    case  5: return;
    }
    break;
  case 2:
    switch(cartridgeSelect) {
    case  6: return;
    case  7: return;
    case  8: return;
    case  9: return;
    case 10: return;
    }
    break;
  case 3:
    switch(cartridgeSelect) {
    case 11: return;
    case 12: return;
    case 13: return;
    case 14: return;
    case 15: return;
    }
    break;
  }
}

auto FamicomBox::chrRead(uint14 addr) -> uint8 {
  if(cartridgeRowSelect == 0 && cartridgeSelect == 0) {
    return bios_chr[addr];
  }
  switch(cartridgeRowSelect) {
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.readCHR(addr);
    case  2: break;
    case  3: break;
    case  4: break;
    case  5: break;
    }
    break;
  case 2:
    switch(cartridgeSelect) {
    case  6: break;
    case  7: break;
    case  8: break;
    case  9: break;
    case 10: break;
    }
    break;
  case 3:
    switch(cartridgeSelect) {
    case 11: break;
    case 12: break;
    case 13: break;
    case 14: break;
    case 15: break;
    }
    break;
  }
  return ppu.io.mdr;
}

auto FamicomBox::chrWrite(uint14 addr, uint8 data) -> void {
  switch(cartridgeRowSelect) {
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.writeCHR(addr, data);
    case  2: break;
    case  3: break;
    case  4: break;
    case  5: break;
    }
    break;
  case 2:
    switch(cartridgeSelect) {
    case  6: break;
    case  7: break;
    case  8: break;
    case  9: break;
    case 10: break;
    }
    break;
  case 3:
    switch(cartridgeSelect) {
    case 11: break;
    case 12: break;
    case 13: break;
    case 14: break;
    case 15: break;
    }
    break;
  }
}

}
