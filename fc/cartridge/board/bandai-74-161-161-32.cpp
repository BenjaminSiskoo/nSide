//BANDAI-74*161/161/32

struct Bandai74_161_161_32 : Board {

struct Settings {
  bool mirror;  //0 = vertical, 1 = horizontal
} settings;

uint4 prg_bank;
uint4 chr_bank;

uint8 prg_read(unsigned addr) {
  if((addr & 0x8000) == 0x0000) return cpu.mdr();
  if((addr & 0xc000) == 0x8000) return prgrom.read((prg_bank << 14) | (addr & 0x3fff));
  else                          return prgrom.read((    0x0f << 14) | (addr & 0x3fff));
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    //TODO: check for bus conflicts
    data &= prg_read(addr);
    prg_bank = (data & 0xf0) >> 4;
    chr_bank = (data & 0x0f) >> 0;
  }
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) {
    if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_read(addr);
  }
  addr = (chr_bank * 0x2000) + (addr & 0x1fff);
  return Board::chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) {
    if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_write(addr, data);
  }
  addr = (chr_bank * 0x2000) + (addr & 0x1fff);
  Board::chr_write(addr, data);
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

Bandai74_161_161_32(Markup::Node& cartridge) : Board(cartridge) {
  settings.mirror = cartridge["mirror/mode"].data == "horizontal";
}

};
