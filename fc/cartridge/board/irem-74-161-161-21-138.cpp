//IREM-74*161/161/21/138

struct Irem74_161_161_21_138 : Board {

uint4 prg_bank;
uint4 chr_bank;

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    // Bus conflicts
    data &= prg_read(addr);
    prg_bank = (data & 0x0f) >> 0;
    chr_bank = (data & 0xf0) >> 4;
  }
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ciram_read(addr);
  if((addr & 0x1800) == 0x0000) return read(chrrom, (addr & 0x07ff) | (chr_bank << 11));
  return read(chrram, addr);
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ciram_write(addr, data);
  if((addr & 0x1800) != 0x0000) return write(chrram, addr & 0x1fff, data);
}

uint8 ciram_read(unsigned addr) {
  switch(addr & 0x0800) {
  case 0x0000: return read(chrram, addr & 0x1fff);
  case 0x0800: return ppu.ciram_read(addr);
  }
}

void ciram_write(unsigned addr, uint8 data) {
  switch(addr & 0x0800) {
  case 0x0000: return write(chrram, addr & 0x1fff, data);
  case 0x0800: return ppu.ciram_write(addr, data);
  }
}

void power() {
}

void reset() {
  prg_bank = 0;
  chr_bank = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);
  s.integer(prg_bank);
  s.integer(chr_bank);
}

Irem74_161_161_21_138(Markup::Node& cartridge) : Board(cartridge) {
}

};
