#include <ms/ms.hpp>

namespace MasterSystem {

CPU cpu;

auto CPU::Enter() -> void {
  while(true) scheduler.synchronize(), cpu.main();
}

auto CPU::main() -> void {
  instruction();
}

auto CPU::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(vdp);
  synchronize(psg);
}

auto CPU::wait() -> void {
  step(1);
}

auto CPU::read(uint16 addr) -> uint8 {
  step(1);
  if(addr < 0xc000 || ramDisable) return cartridge.read(addr);
  return ram[addr & ramMask];
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  step(1);
  if(addr < 0xc000 || ramDisable) return cartridge.write(addr, data);
  ram[addr & ramMask] = data;
}

auto CPU::portRead(uint8 port) -> uint8 {
  if((port & 0xc1) == 0x00) return 0xff;  //port $3e
  if((port & 0xc1) == 0x40) return 0x00/*vdp.hcounter*/;  //port $7e
  if((port & 0xc1) == 0x41) return 0x00/*vdp.vcounter*/;  //port $7f
  if((port & 0xc1) == 0x80) return vdp.readData();      //port $be
  if((port & 0xc1) == 0x81) return vdp.readControl();   //port $bf
  if((port & 0xc0) == 0xc0) {
    uint7 data0 = MasterSystem::peripherals.controllerPort1->readData();
    uint7 data1 = MasterSystem::peripherals.controllerPort2->readData();
    uint8 data;
    if((port & 0xc1) == 0xc0) {  //port $dc
      data.bits(0,5) = (uint)data0.bits(0,5);
      data.bits(6,7) = (uint)data1.bits(0,1);
    }
    if((port & 0xc1) == 0xc1) {  //port $dd
      data.bits(0,3) = (uint)data1.bits(2,5);
      data.bit (  4) = 1;  //reset button
      data.bit (  5) = 1;  //cartridge CONT pin
      data.bit (  6) = (uint)data0.bit (6);
      data.bit (  7) = (uint)data1.bit (6);
    }
    return data;
  }
}

auto CPU::portWrite(uint8 port, uint8 data) -> void {
  switch(port & 0xc1) {

  case 0x00: {  //port $3e
    ioDisable        = data.bit(2);
    biosDisable      = data.bit(3);
    ramDisable       = data.bit(4);
    cardDisable      = data.bit(5);
    cartridgeDisable = data.bit(6);
    expansionDisable = data.bit(7);
    break;
  }

  case 0x01: {  //port $3f
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
    break;
  }

  case 0x80: {  //port $be
    vdp.writeData(data);
    break;
  }

  case 0x81: {  //port $bf
    vdp.writeControl(data);
    break;
  }

  }
}

auto CPU::stop() -> bool {
}

auto CPU::power() -> void {
  ramMask = system.model() == Model::SG1000 ? 0x3ff : 0x1fff;
}

auto CPU::reset() -> void {
  create(CPU::Enter, system.colorburst() * 6.0);
}

}
