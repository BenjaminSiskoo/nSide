auto PPU::readCIRAM(uint12 addr) -> uint8 {
  if(!system.vs()) addr &= 0x7ff;
  return ciram[addr];
}

auto PPU::writeCIRAM(uint12 addr, uint8 data) -> void {
  if(!system.vs()) addr &= 0x7ff;
  ciram[addr] = data;
}

auto PPU::readCGRAM(uint5 addr) -> uint8 {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  uint8 data = cgram[addr];
  if(r.grayscale) data &= 0x30;
  return data;
}

auto PPU::writeCGRAM(uint5 addr, uint8 data) -> void {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  cgram[addr] = data;
}

auto PPU::readOAM(uint8 addr) -> uint8 {
  uint8 data = oam[addr];
  return data;
}

auto PPU::writeOAM(uint8 addr, uint8 data) -> void {
  if(addr.bits(0,1) == 2) data.bits(2,4) = 0;  //clear non-existent bits (always read back as 0)
  oam[addr] = data;
}
