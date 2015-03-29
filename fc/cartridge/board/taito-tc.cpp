struct TaitoTC : Board {

TC tc;

void enter() {
  tc.enter();
}

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return read(prgrom, tc.prg_addr(addr));
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) return tc.reg_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ppu.ciram_read(tc.ciram_addr(addr));
  return Board::chr_read(tc.chr_addr(addr));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ppu.ciram_write(tc.ciram_addr(addr), data);
  return Board::chr_write(tc.chr_addr(addr), data);
}

void power() {
  tc.power();
}

void reset() {
  tc.reset();
}

void serialize(serializer& s) {
  Board::serialize(s);
  tc.serialize(s);
}

TaitoTC(Markup::Node& cartridge) : Board(cartridge), tc(*this, cartridge) {
}

};
