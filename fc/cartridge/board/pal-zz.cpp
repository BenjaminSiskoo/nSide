struct PAL_ZZ : Board {

MMC3 mmc3;
uint2 prg_a16_force;
bool bank;

void enter() {
  mmc3.enter();
}

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) {
    addr = (mmc3.prg_addr(addr) & (0xffff | (bank << 16))) | (bank << 17);
    addr |= (prg_a16_force == 0x03) << 16;
    return read(prgrom, addr);
  }
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if((addr & 0xe000) == 0x6000 && mmc3.ram_enable && !mmc3.ram_write_protect) {
    prg_a16_force = data & 0x03;
    bank = data & 0x04;
  }
  if(addr & 0x8000) return mmc3.reg_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ppu.ciram_read(mmc3.ciram_addr(addr));
  addr = (mmc3.chr_addr(addr) & 0x1ffff) | (bank << 17);
  return Board::chr_read(mmc3.chr_addr(addr));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ppu.ciram_write(mmc3.ciram_addr(addr), data);
  addr = (mmc3.chr_addr(addr) & 0x1ffff) | (bank << 17);
  return Board::chr_write(mmc3.chr_addr(addr), data);
}

unsigned ciram_addr(unsigned addr) {
  return mmc3.ciram_addr(addr);
}

void power() {
  mmc3.power();
}

void reset() {
  mmc3.reset();
  cic_reset();
}

void cic_reset() {
  // this register is cleared by the CIC reset line.
  // On a Famicom or toploader, only a power cycle can clear it.
  //TODO: Check if Europe got the toploader.
  prg_a16_force = 0;
  bank = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);
  mmc3.serialize(s);
  s.integer(prg_a16_force);
  s.integer(bank);
}

PAL_ZZ(Markup::Node& cartridge) : Board(cartridge), mmc3(*this, cartridge) {
}

};
