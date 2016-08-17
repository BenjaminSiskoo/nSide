#include <ms/ms.hpp>

namespace MasterSystem {

CPU cpu;

auto CPU::Enter() -> void {
  while(true) scheduler.synchronize(), cpu.main();
}

auto CPU::main() -> void {
  step(6);
}

auto CPU::step(uint clocks) -> void {
  Thread::step(clocks);
}

auto CPU::stop() -> bool {
}

auto CPU::power() -> void {
  ramMask = system.model() == Model::SG1000 ? 0x3ff : 0x1fff;
}

auto CPU::reset() -> void {
  create(CPU::Enter, system.colorburst() * 6.0);
}

auto CPU::idle() -> void {
}

auto CPU::read(uint16 addr) -> uint8 {
  if(addr < 0xc000 || ramDisable) return cartridge.read(addr);
  else                            return ram[addr & ramMask];
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  if(addr < 0xc000 || ramDisable) cartridge.write(addr, data);
  else                            ram[addr & ramMask] = data;
}

auto CPU::portRead(uint8 port) -> uint8 {
  if((port & 0xc1) == 0x00) return 0xff;  //port $3e
  if((port & 0xc1) == 0x40) return 0x00/*vdp.hcounter*/;  //port $7e
  if((port & 0xc1) == 0x41) return 0x00/*vdp.vcounter*/;  //port $7f
  if((port & 0xc1) == 0x80) return vdp.readData();      //port $be
  if((port & 0xc1) == 0x81) return vdp.readControl();   //port $bf
  if((port & 0xc0) == 0xc0) {
    uint8 data0 = MasterSystem::peripherals.controllerPort1->read();
    uint8 data1 = MasterSystem::peripherals.controllerPort2->read();
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
    io.port[0].trDirection = data.bit(0);
  //io.port[0].thDirection = data.bit(1);
    io.port[1].trDirection = data.bit(2);
  //io.port[1].thDirection = data.bit(3);
    if(!io.port[0].trDirection) io.port[0].trOutput = data.bit(4);
  //if(!io.port[0].thDirection) io.port[0].thOutput = data.bit(5);
    if(!io.port[1].trDirection) io.port[1].trOutput = data.bit(6);
  //if(!io.port[1].thDirection) io.port[1].thOutput = data.bit(7);
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

}
