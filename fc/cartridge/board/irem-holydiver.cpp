//IREM-HOLYDIVER

struct IremHolyDiver : Board {

uint3 prg_bank;
uint4 chr_bank;
bool mirror;

uint8 prg_read(unsigned addr) {
  switch(addr & 0xc000) {
  case 0x8000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
  case 0xc000: return read(prgrom, (    0x07 << 14) | (addr & 0x3fff));
  }
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    // Bus conflicts
    data &= prg_read(addr);
    prg_bank = (data & 0x07) >> 0;
    mirror = data & 0x08;
    chr_bank = (data & 0xf0) >> 4;
  }
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) {
    if(mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_read(addr);
  }
  return Board::chr_read((chr_bank * 0x2000) + (addr & 0x1fff));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) {
    if(mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_write(addr, data);
  }
  Board::chr_write((chr_bank * 0x2000) + (addr & 0x1fff), data);
}

void power() {
}

void reset() {
  prg_bank = 0;
  chr_bank = 0;
  mirror = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);
  s.integer(prg_bank);
  s.integer(chr_bank);
  s.integer(mirror);
}

IremHolyDiver(Markup::Node& cartridge) : Board(cartridge) {
}

};
