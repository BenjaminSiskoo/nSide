struct IremH3001 : Board {

IFH3001 ifh3001;

void enter() {
  ifh3001.enter();
}

uint8 prg_read(unsigned addr) {
  if((addr & 0x8000) == 0x8000) return read(prgrom, ifh3001.prg_addr(addr));
  if((addr & 0xe000) == 0x6000) return read(prgram, addr & 0x1fff);
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if((addr & 0x8000) == 0x8000) return ifh3001.reg_write(addr, data);
  if((addr & 0xe000) == 0x6000) return write(prgram, addr & 0x1fff, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ppu.ciram_read(ifh3001.ciram_addr(addr));
  return Board::chr_read(ifh3001.chr_addr(addr));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ppu.ciram_write(ifh3001.ciram_addr(addr), data);
  return Board::chr_write(ifh3001.chr_addr(addr), data);
}

void power() {
  ifh3001.power();
}

void reset() {
  ifh3001.reset();
}

void serialize(serializer& s) {
  Board::serialize(s);
  ifh3001.serialize(s);
}

IremH3001(Markup::Node& cartridge) : Board(cartridge), ifh3001(*this, cartridge) {
}

};
