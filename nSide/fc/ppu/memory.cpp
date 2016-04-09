auto PPU::ciramRead(uint14 addr) -> uint8 {
  return ciram[addr & (!system.vs() ? 0x07ff : 0x0fff)];
}

auto PPU::ciramWrite(uint14 addr, uint8 data) -> void {
  ciram[addr & (!system.vs() ? 0x07ff : 0x0fff)] = data;
}

auto PPU::cgramRead(uint14 addr) -> uint8 {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  uint8 data = cgram[addr & 0x1f];
  if(status.grayscale) data &= 0x30;
  return data;
}

auto PPU::cgramWrite(uint14 addr, uint8 data) -> void {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  cgram[addr & 0x1f] = data;
}

auto PPU::oamRead(uint8 addr) -> uint8 {
  uint8 data = oam[addr];
  return data;
}

auto PPU::oamWrite(uint8 addr, uint8 data) -> void {
  if(addr.bits(0,1) == 2) data.bits(2,4) = 0;  //clear non-existent bits (always read back as 0)
  oam[addr] = data;
}
