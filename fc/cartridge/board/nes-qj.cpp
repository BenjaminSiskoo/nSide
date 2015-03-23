struct NES_QJ : Board {

MMC3 mmc3;
bool bank;

void enter() {
  mmc3.enter();
}

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) {
    addr = (mmc3.prg_addr(addr) & 0x1ffff) | (bank << 17);
    return read(prgrom, addr);
  }
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if((addr & 0xe000) == 0x6000 && mmc3.ram_enable && !mmc3.ram_write_protect) {
    bank = data & 0x01;
  }
  if(addr & 0x8000) return mmc3.reg_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ppu.ciram_read(mmc3.ciram_addr(addr));
  return Board::chr_read((mmc3.chr_addr(addr) & 0x1ffff) | (bank << 17));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ppu.ciram_write(mmc3.ciram_addr(addr), data);
  return Board::chr_write((mmc3.chr_addr(addr) & 0x1ffff) | (bank << 17), data);
}

unsigned ciram_addr(unsigned addr) {
  return mmc3.ciram_addr(addr);
}

void power() {
  mmc3.power();
}

void reset() {
  mmc3.reset();
  bank = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);
  mmc3.serialize(s);
  s.integer(bank);
}

NES_QJ(Markup::Node& cartridge) : Board(cartridge), mmc3(*this, cartridge) {
}

};
