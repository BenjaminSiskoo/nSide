#include <ms/ms.hpp>

namespace MasterSystem {

Bus bus;

auto Bus::read(uint16 addr) -> uint8 {
  auto data = cartridge.read(addr);
  if(addr >= 0xc000 && !disableRAM) return ram[addr & ramMask];
  return data ? data() : (uint8)0x00;
}

auto Bus::write(uint16 addr, uint8 data) -> void {
  cartridge.write(addr, data);
  if(addr >= 0xc000 && !disableRAM) ram[addr & ramMask] = data;
}

auto Bus::in(uint8 addr) -> uint8 {
  switch(addr >> 6) {

  case 0: {
    if(system.model() == Model::GameGear && (addr < 0x06)) {
      if(addr == 0x00) {
        auto hardware = peripherals.hardware->readData();
        return hardware.bit(6) << 7 | 0x7f;
      }
      return 0xff;  //Ports $03, $04, and $05 are for the Gear to Gear Cable
    } else if(!addr.bit(0)) {  //officially port $3e
      uint8 data = 0x00;
      data.bit(2) = disableIO;
      data.bit(3) = disableBIOS;
      data.bit(4) = disableRAM;
      data.bit(5) = disableMyCard;
      data.bit(6) = disableCartridge;
      data.bit(7) = disableExpansion;
      return data;
    } else {
      return 0xff;  //SMS1 = MDR, SMS2 = 0xff
    }
  }

  case 1: {  //officially ports $7e and $7f
    return !addr.bit(0) ? vdp.vcounter() : vdp.hcounter();
  }

  case 2: {  //officially ports $be and $bf
    return !addr.bit(0) ? vdp.data() : vdp.status();
  }

  case 3: {
    switch(system.model()) {

    case Model::SG1000: {
      auto port1 = peripherals.controllerPort1->readData();
      auto port2 = peripherals.controllerPort2->readData();
      if(addr.bit(0) == 0) {
        return port1.bits(0,5) << 0 | port2.bits(0,1) << 6;
      } else {
        return port2.bits(2,5) << 0 | 1 << 4 | 1 << 5 | port1.bit(6) << 6 | port2.bit(6) << 7;
      }
    }

    case Model::MasterSystem: {
      auto hardware = peripherals.hardware->readData();
      auto port1 = peripherals.controllerPort1->readData();
      auto port2 = peripherals.controllerPort2->readData();
      if(addr.bit(0) == 0) {
        return port1.bits(0,5) << 0 | port2.bits(0,1) << 6;
      } else {
        return port2.bits(2,5) << 0 | hardware.bit(1) << 4 | 1 << 5 | port1.bit(6) << 6 | port2.bit(6) << 7;
      }
    }

    case Model::GameGear: {
      auto hardware = peripherals.hardware->readData();
      if(addr.bit(0) == 0) {
        return hardware.bits(0,5) << 0 | 0xc0;
      } else {
        return 0xff;
      }
    }

    }

    unreachable;
  }

  }

  unreachable;
}

auto Bus::out(uint8 addr, uint8 data) -> void {
  switch(addr >> 6) {

  case 0: {
    if(!addr.bit(0)) {
      disableIO        = data.bit(2);
      disableBIOS      = data.bit(3);
      disableRAM       = data.bit(4);
      disableMyCard    = data.bit(5);
      disableCartridge = data.bit(6);
      disableExpansion = data.bit(7);
    } else {
      //Writing to TH lines has no effect in Japanese systems.
      /*
      uint7 control1;
      uint7 control2;
      uint7 data1;
      uint7 data2;
      control1.bit(5) = data.bit(0);
      control1.bit(6) = data.bit(1);
      control2.bit(5) = data.bit(2);
      control2.bit(6) = data.bit(3);
      data1.bit(5) = data.bit(4);
      data1.bit(6) = data.bit(5);
      data2.bit(5) = data.bit(6);
      data2.bit(6) = data.bit(7);
      MasterSystem::peripherals.controllerPort1->writeControl(control1);
      MasterSystem::peripherals.controllerPort2->writeControl(control2);
      MasterSystem::peripherals.controllerPort1->writeData(data1);
      MasterSystem::peripherals.controllerPort2->writeData(data2);
      */
    }
    break;
  }

  case 1: {  //officially port $7f
    return psg.data(data);
  }

  case 2: {  //officially ports $be and $bf
    return !addr.bit(0) ? vdp.data(data) : vdp.control(data);
  }

  case 3: {
    return;  //unmapped
  }

  }
}

auto Bus::power() -> void {
  ramMask = system.model() == Model::SG1000 ? 0x3ff : 0x1fff;
  disableIO        = false;
  disableBIOS      = true;
  disableRAM       = false;
  disableMyCard    = true;
  disableCartridge = false;
  disableExpansion = true;
}

}
