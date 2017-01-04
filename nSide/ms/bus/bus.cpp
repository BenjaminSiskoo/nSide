#include <ms/ms.hpp>

namespace MasterSystem {

Bus bus;

auto Bus::read(uint16 addr) -> uint8 {
  if(addr < 0xc000 || disableRAM) return cartridge.read(addr);
  return ram[addr & ramMask];
}

auto Bus::write(uint16 addr, uint8 data) -> void {
  if(addr < 0xc000 || disableRAM) return cartridge.write(addr, data);
  ram[addr & ramMask] = data;
}

auto Bus::in(uint8 addr) -> uint8 {
  switch(addr >> 6) {

  case 0: {  //officially port $3e
    if(system.model() == Model::GameGear && (addr < 0x06)) {
      return 0xff;  //Gear to Gear Cable
    } else if(!addr.bit(0)) {
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
    if(system.model() == Model::GameGear && (addr != 0xc0 && addr != 0xdc)) return 0x00;
    uint7 data0 = MasterSystem::peripherals.controllerPort1->readData();
    uint7 data1 = MasterSystem::peripherals.controllerPort2->readData();
    uint8 data = 0x00;
    if(!addr.bit(0)) {
      data.bits(0,5) = (uint)data0.bits(0,5);
      data.bits(6,7) = (uint)data1.bits(0,1);
    } else {
      data.bits(0,3) = (uint)data1.bits(2,5);
      data.bit (  4) = 1;  //reset button
      data.bit (  5) = 1;  //cartridge CONT pin
      data.bit (  6) = (bool)data0.bit (6);
      data.bit (  7) = (bool)data1.bit (6);
    }
    return data;
  }

  }

  return 0x00;
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
    }
    break;
  }

  case 1: {  //officially port $7f
    return psg.out(addr, data);
  }

  case 2: {  //officially ports $be and $bf
    return !addr.bit(0) ? vdp.data(data) : vdp.control(data);
  }

  case 3: {
    return;  //unmapped
  }

  }
}

}
