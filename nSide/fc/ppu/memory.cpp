auto PPU::readCIRAM(uint14 addr) -> uint8 {
  return ciram[addr & (!system.vs() ? 0x07ff : 0x0fff)];
}

auto PPU::writeCIRAM(uint14 addr, uint8 data) -> void {
  ciram[addr & (!system.vs() ? 0x07ff : 0x0fff)] = data;
}

auto PPU::readCGRAM(uint5 addr) -> uint8 {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  uint8 data = cgram[addr & 0x1f];
  if(r.grayscale) data &= 0x30;
  return data;
}

auto PPU::writeCGRAM(uint5 addr, uint8 data) -> void {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  cgram[addr & 0x1f] = data;
}

auto PPU::readOAM(uint8 addr) -> uint8 {
  uint8 data = oam[addr];
  return data;
}

auto PPU::writeOAM(uint8 addr, uint8 data) -> void {
  if(addr.bits(0,1) == 2) data.bits(2,4) = 0;  //clear non-existent bits (always read back as 0)
  oam[addr] = data;
}
