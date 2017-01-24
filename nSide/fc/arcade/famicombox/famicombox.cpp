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
    if(++watchdog        == 0x0000) trap(Exception::Watchdog);
    counter = 3 * 0x2000;
  }
  step(1);
  if(clock() - cpu0.clock() > Thread::Second / 1'000) synchronize(cpu0);
}

auto FamicomBox::load(Markup::Node node) -> bool {
  if(auto bios_prg = node["prg/rom/name"].text()) {
    if(auto fp = platform->open(ID::System, bios_prg, File::Read, File::Required)) {
      fp->read(famicombox.bios_prg, 32768);
    } else return false;
  }

  if(auto bios_chr = node["chr/rom/name"].text()) {
    if(auto fp = platform->open(ID::System, bios_chr, File::Read, File::Required)) {
      fp->read(famicombox.bios_chr, 8192);
    } else return false;
  }

  dip = platform->dipSettings(node);
  keyswitch = 1 << (1 - 1);

  return true;
}

auto FamicomBox::power() -> void {
  create(FamicomBox::Enter, system.colorburst() * 6.0);

  exceptionEnable = 0x00;

  zapperGND = false;
  warmboot = false;
  enableControllers = true;
  swapControllers = true;

  cartridgeSelect = 0;
  cartridgeRowSelect = 0;

  function<auto (uint16, uint8) -> uint8> reader;
  function<auto (uint16, uint8) -> void> writer;

  reader = {&FamicomBox::readSRAM, this};
  writer = {&FamicomBox::writeSRAM, this};
  bus0.map(reader, writer, "6000-7fff");

  reader = {&FamicomBox::readCartridge, this};
  writer = {&FamicomBox::writeCartridge, this};
  bus0.map(reader, writer, "8000-ffff");
  // The cartridge is only mapped to $8000-ffff, not $4018-ffff.

  cpu0.reset();  //Workaround for CPU jumping to reset vector at $fffc before FamicomBox

  reset();
}

auto FamicomBox::reset() -> void {
  function<auto (uint16, uint8) -> uint8> reader;
  function<auto (uint16, uint8) -> void> writer;

  reader = {&FamicomBox::readWRAM, this};
  writer = {&FamicomBox::writeWRAM, this};
  bus0.map(reader, writer, "0800-1fff");

  reader = {&FamicomBox::readIO, this};
  writer = {&FamicomBox::writeIO, this};
  bus0.map(reader, writer, "4016-4017");
  bus0.map(reader, writer, "5000-5fff");

  exceptionTrap = 0xff;
  ramProtect = 7;
  counter = 3 * 0x2000;
  attractionTimer.bits(7,14) = 0xff;
  watchdog.bits(10,13) = 0x00;

  cartridgeSelect = 0;
  cartridgeRowSelect = 0;
  registerLock = false;
}

auto FamicomBox::trap(Exception exceptionId) -> void {
  if(exceptionId != Exception::ControllerRead) print("Trap Exception ", (uint)exceptionId, "\n");
  if(!exceptionEnable.bit((uint)exceptionId)) return;
  exceptionTrap.bit((uint)exceptionId) = 0;
  synchronize(cpu0);
  synchronize(apu0);
  synchronize(cartridge);
  cartridge.reset();
  cpu0.reset();
  apu0.reset();
  reset();
}

auto FamicomBox::pollInputs() -> void {
  static bool resetButton = false;
  static bool keyswitchLeft = false;
  static bool keyswitchRight = false;
  static bool coin = false;
  bool state;

  state = platform->inputPoll(ID::Port::Hardware, ID::Device::FamicomBoxControls, 0);
  if(state && !resetButton) trap(Exception::Reset);
  resetButton = state;

  state = platform->inputPoll(ID::Port::Hardware, ID::Device::FamicomBoxControls, 1);
  if(state && !keyswitchLeft) {
    if(keyswitch != 1 << 0) keyswitch >>= 1;
    trap(Exception::KeyswitchRotate);
  }
  keyswitchLeft = state;

  state = platform->inputPoll(ID::Port::Hardware, ID::Device::FamicomBoxControls, 2);
  if(state && !keyswitchRight) {
    if(keyswitch != 1 << 5) keyswitch <<= 1;
    trap(Exception::KeyswitchRotate);
  }
  keyswitchRight = state;

  state = platform->inputPoll(ID::Port::Hardware, ID::Device::FamicomBoxControls, 3);
  if(state && !coin) trap(Exception::Coin);
  coin = state;
}

auto FamicomBox::readWRAM(uint16 addr, uint8 data) -> uint8 {
  switch(addr & 0xf800) {
  case 0x0000: return cpu0.ram[addr];
  case 0x0800:
  case 0x1000:
  case 0x1800: return bios_ram[addr - 0x800];
  }
}

auto FamicomBox::readIO(uint16 addr, uint8 data) -> uint8 {
  if(addr == 0x4016 || addr == 0x4017) {
    watchdog.bits(10,13) = 0;
    if(!enableControllers) return data;
    if(addr == 0x4017) data.bits(3,4) = dip.bit(9) ? Famicom::peripherals.expansionPort->data2().bits(1,2) : 0;
    if(swapControllers) addr ^= 1;
    switch(addr) {
    case 0x4016: data.bit(0) = Famicom::peripherals.controllerPort1->data().bit(0); break;
    case 0x4017: data.bit(0) = Famicom::peripherals.controllerPort2->data().bit(0); break;
    }
    trap(Exception::ControllerRead);
    return data;
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
      return dip.bits(0,7);
    }

    //Keyswitch position and coin module status
    case 0x5003: {
      return keyswitch;
    }

    //Test connector (25-pin) inputs 2, 15, 3, 16, 4, 17, 5, 18
    case 0x5004: {
      return data;
    }

    //Expansion connector (50-pin) read signal on input pin 28
    case 0x5005: {
      //Data comes through pins 36, 11, 37, 12, 38, 13, 39, and 14.
      return data;
    }

    //Expansion connector (50-pin) read signal on input pin 27
    case 0x5006: {
      //Data comes through pins 36, 11, 37, 12, 38, 13, 39, and 14.
      return data;
    }

    //Misc. status
    case 0x5007: {
      data.bit(0) = 0;  //Must be 0. TODO: check if 1 is TV mode or a trap flag
      data.bit(1) = 1;
      data.bit(2) = !zapperGND && settings.expansionPort == ID::Device::BeamGun;
      //data.bit(3);  //Expansion connector (50-pin) input pin 21 inverted
      data.bit(4) = 0;
      data.bit(5) = 0;
      //data.bit(6);  //Expansion connector (50-pin) input pin 22 inverted
      data.bit(7) = !warmboot;
      return data;
    }

    }
  }

  return data;
}

auto FamicomBox::readSRAM(uint16 addr, uint8 data) -> uint8 {
  return test_ram[addr & 0x1fff];
}

auto FamicomBox::readCartridge(uint16 addr, uint8 data) -> uint8 {
  switch(cartridgeRowSelect) {
  case 0:
    if(cartridgeSelect == 0) return bios_prg[addr & 0x7fff];
    break;
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.readPRG(addr, data);
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

auto FamicomBox::writeWRAM(uint16 addr, uint8 data) -> void {
  switch(addr & 0xf800) {
  case 0x0000: {
    if(ramProtect >= 1) cpu0.ram[addr] = data;
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

auto FamicomBox::writeIO(uint16 addr, uint8 data) -> void {
  if(addr == 0x4016 || addr == 0x4017 && enableControllers) return cpu0.writeCPU(addr, data);

  if(!registerLock) {
    switch(addr & 0xf007) {

    case 0x5000: {  //Exception enable flags
      data ^ (1 << (uint)Exception::Interrupt6_82Hz);  //Exception disable flag
      exceptionEnable = (data & 0xbf) | (1 << (uint)Exception::Watchdog);
      break;
    }

    case 0x5001: {  //Coin module flags and CATV output
      break;
    }

    case 0x5002: {  //Cartridge slot LED and RAM protect register
      ramProtect = data.bits(4,6);
      break;
    }

    case 0x5003: {  //Attraction timer
      attractionTimer.bits(7,14) = data;
      break;
    }

    case 0x5004: {  //Cartridge selection register
      cartridgeSelect = data.bits(0,3);
      cartridgeRowSelect = data.bits(4,5);
      registerLock |= data.bit(6);
      break;
    }

    case 0x5005: {  //Misc. control
      zapperGND = data.bit(2);
      warmboot = data.bit(5);
      enableControllers = !data.bit(6);
      swapControllers = !data.bit(7);
      break;
    }

    case 0x5006: {  //Test connector DB-25 outputs 6, 19, 7, 20, 8, 21, 9, 22
      break;
    }

    case 0x5007: {  //Expansion connector (50-pin) write signal on pin 26
      //Data goes through pins 36, 11, 37, 12, 38, 13, 39, and 14.
      break;
    }

    }
  }
}

auto FamicomBox::writeSRAM(uint16 addr, uint8 data) -> void {
  test_ram[addr & 0x1fff] = data;
}

auto FamicomBox::writeCartridge(uint16 addr, uint8 data) -> void {
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

auto FamicomBox::readCHR(uint14 addr, uint8 data) -> uint8 {
  if(cartridgeRowSelect == 0 && cartridgeSelect == 0) {
    if(addr & 0x2000) return ppu0.readCIRAM((addr & 0x3ff) | (addr & 0x800) >> 1);
    return bios_chr[addr];
  }
  switch(cartridgeRowSelect) {
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.readCHR(addr, data);
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

auto FamicomBox::writeCHR(uint14 addr, uint8 data) -> void {
  if(cartridgeRowSelect == 0 && cartridgeSelect == 0) {
    if(addr & 0x2000) return ppu0.writeCIRAM((addr & 0x3ff) | (addr & 0x800) >> 1, data);
  }
  switch(cartridgeRowSelect) {
  case 1:
    switch(cartridgeSelect) {
    case  1: return cartridge.writeCHR(addr, data);
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

}
