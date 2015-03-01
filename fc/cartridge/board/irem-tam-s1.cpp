//IREM-TAM-S1

struct IremTamS1 : Board {

uint4 prg_bank;
uint2 mirror; //0 = screen 0, 1 = horizontal, 2 = vertical, 3 = screen 1

uint8 prg_read(unsigned addr) {
  if((addr & 0x8000) == 0x0000) return cpu.mdr();
  switch(addr & 0xc000) {
  case 0x8000: return read(prgrom, (    0x0f << 14) | (addr & 0x3fff));
  case 0xc000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
  }
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    //TODO: check for bus conflicts
    //data &= prg_read(addr);
    prg_bank = data & 0x0f;
    mirror = (data & 0xc0) >> 6;
  }
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) {
    switch(mirror) {
    case 0: addr = (0x0000              ) | (addr & 0x03ff); break;
    case 1: addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff); break;
    case 2: addr = ((addr & 0x0400) >> 0) | (addr & 0x03ff); break;
    case 3: addr = (0x0400              ) | (addr & 0x03ff); break;
    }
    return ppu.ciram_read(addr);
  }
  return Board::chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) {
    switch(mirror) {
    case 0: addr = (0x0000              ) | (addr & 0x03ff); break;
    case 1: addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff); break;
    case 2: addr = ((addr & 0x0400) >> 0) | (addr & 0x03ff); break;
    case 3: addr = (0x0400              ) | (addr & 0x03ff); break;
    }
    return ppu.ciram_write(addr, data);
  }
  return Board::chr_write(addr, data);
}

void power() {
}

void reset() {
  prg_bank = 0;
  mirror = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);

  s.integer(prg_bank);
  s.integer(mirror);
}

IremTamS1(Markup::Node& cartridge) : Board(cartridge) {
}

};
