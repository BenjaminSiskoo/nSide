#include <md/md.hpp>

namespace MegaDrive {

BusCPU busCPU;
BusAPU busAPU;

auto BusCPU::readByte(uint24 addr) -> uint16 {
  if(addr < 0x400000) return cartridge.read(addr & ~1).byte(!addr.bit(0));
  if(addr < 0xa00000) return 0x0000;
  if(addr < 0xa10000) return busAPU.granted() ? busAPU.read(addr) : (uint8)0x0000;
  if(addr < 0xa11000) return readIO(addr & ~0xff00);
  if(addr < 0xa12000) return readIO(addr & ~0x00ff);
  if(addr < 0xc00000) return 0x0000;
  if(addr < 0xe00000) return vdp.read(addr & ~1).byte(!addr.bit(0));
  return ram[addr & 0xffff];
}

auto BusCPU::readWord(uint24 addr) -> uint16 {
  if(addr < 0x400000) return cartridge.read(addr);
  if(addr < 0xa00000) return 0x0000;
  if(addr < 0xa10000) return busAPU.granted() ? busAPU.read(addr) : (uint8)0x0000;
  if(addr < 0xa11000) return readIO(addr & ~0xff00) << 0;
  if(addr < 0xa12000) return readIO(addr & ~0x00ff) << 8;
  if(addr < 0xc00000) return 0x0000;
  if(addr < 0xe00000) return vdp.read(addr);
  uint16 data = ram[addr + 0 & 0xffff] << 8;
  return data | ram[addr + 1 & 0xffff] << 0;
}

auto BusCPU::writeByte(uint24 addr, uint16 data) -> void {
  if(addr < 0x400000) return cartridge.write(addr & ~1, data << 8 | data << 0);
  if(addr < 0xa00000) return;
  if(addr < 0xa10000) return busAPU.granted() ? busAPU.write(addr, data) : (void)0;
  if(addr < 0xa11000) return writeIO(addr & ~0xff00, data);
  if(addr < 0xa12000) return writeIO(addr & ~0x00ff, data);
  if(addr < 0xc00000) return;
  if(addr < 0xc00010) return vdp.write(addr & ~1, data << 8 | data << 0);
  if(addr < 0xc00018) return psg.write(data);
  if(addr < 0xe00000) return;
  ram[addr & 0xffff] = data;
}

auto BusCPU::writeWord(uint24 addr, uint16 data) -> void {
  if(addr < 0x400000) return cartridge.write(addr, data);
  if(addr < 0xa00000) return;
  if(addr < 0xa10000) return busAPU.granted() ? busAPU.write(addr, data) : (void)0;
  if(addr < 0xa11000) return writeIO(addr & ~0xff00, data >> 0);
  if(addr < 0xa12000) return writeIO(addr & ~0x00ff, data >> 8);
  if(addr < 0xc00000) return;
  if(addr < 0xc00010) return vdp.write(addr, data);
  if(addr < 0xc00018) return psg.write(data);
  if(addr < 0xe00000) return;
  ram[addr + 0 & 0xffff] = data >> 8;
  ram[addr + 1 & 0xffff] = data >> 0;
}

//

auto BusCPU::readIO(uint24 addr) -> uint16 {
  switch(addr & ~1) {
  case 0xa10000: return (
    !Region::NTSCJ() << 7  //0 = domestic (Japan); 1 = export
  | Region::PAL() << 6     //0 = NTSC; 1 = PAL
  | 1 << 5                 //0 = Mega CD connected; 1 = no expansion connected
  | 0 << 0                 //0 = Model 1; 1 = Model 2+
  );

  case 0xa10002: return peripherals.controllerPort1->readData();
  case 0xa10004: return peripherals.controllerPort2->readData();
  case 0xa10006: return peripherals.extensionPort->readData();

  case 0xa10008: return peripherals.controllerPort1->readControl();
  case 0xa1000a: return peripherals.controllerPort2->readControl();
  case 0xa1000c: return peripherals.extensionPort->readControl();

  case 0xa11100: return !busAPU.granted();
  }

  return 0x0000;
}

auto BusCPU::writeIO(uint24 addr, uint16 data) -> void {
  switch(addr & ~1) {
  case 0xa10002: return peripherals.controllerPort1->writeData(data);
  case 0xa10004: return peripherals.controllerPort2->writeData(data);
  case 0xa10006: return peripherals.extensionPort->writeData(data);

  case 0xa10008: return peripherals.controllerPort1->writeControl(data);
  case 0xa1000a: return peripherals.controllerPort2->writeControl(data);
  case 0xa1000c: return peripherals.extensionPort->writeControl(data);

  case 0xa11100: return busAPU.request(data.bit(0));
  case 0xa11200: return apu.enable(data.bit(0));
  }
}

//

auto BusAPU::read(uint16 addr) -> uint8 {
  if((addr & 0xe000) == 0x0000) {
    return ram[addr];
  }

  if(addr == 0x4000) return ym2612.readStatus();
  if(addr == 0x4001) return ym2612.readStatus();
  if(addr == 0x4002) return ym2612.readStatus();
  if(addr == 0x4003) return ym2612.readStatus();

  if((addr & 0x8000) == 0x8000) {
    return cartridge.read(bank << 15 | (addr & 0x7ffe)).byte(!addr.bit(0));
  }
}

auto BusAPU::write(uint16 addr, uint8 data) -> void {
  if((addr & 0xe000) == 0x0000) {
    ram[addr] = data;
    return;
  }

  if(addr == 0x4000) return ym2612.writeAddress(0 << 8 | data);
  if(addr == 0x4001) return ym2612.writeData(data);
  if(addr == 0x4002) return ym2612.writeAddress(1 << 8 | data);
  if(addr == 0x4003) return ym2612.writeData(data);

  if(addr == 0x6000) {
    //1-bit shift register
    bank = data.bit(0) << 8 | bank >> 1;
    return;
  }

  if(addr == 0x7f11) return psg.write(data);
  if(addr == 0x7f13) return psg.write(data);
  if(addr == 0x7f15) return psg.write(data);
  if(addr == 0x7f17) return psg.write(data);

  if((addr & 0x8000) == 0x8000) {
    //todo: do 8-bit writes mirror to 16-bits?
    return cartridge.write(bank << 15 | (addr & 0x7ffe), data << 8 | data << 0);
  }
}

//unused on Mega Drive
auto BusAPU::in(uint8 addr) -> uint8 {
  return 0x00;
}

//unused on Mega Drive
auto BusAPU::out(uint8 addr, uint8 data) -> void {
}

}
