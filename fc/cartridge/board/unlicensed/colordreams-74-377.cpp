//COLORDREAMS-74*377

struct ColorDreams74_377 : Board {

struct Settings {
  bool mirror;  //0 = vertical, 1 = horizontal
} settings;

uint2 prg_bank;
uint4 chr_bank;

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    // Bus conflicts
    data &= prg_read(addr);
    // PRG and CHR bits are swapped relative to NES-GxROM.
    // Additionally, up to 16 CHR banks are available instead of 4.
    prg_bank = (data & 0x03) >> 0;
    chr_bank = (data & 0xf0) >> 4;
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

ColorDreams74_377(Markup::Node& cartridge) : Board(cartridge) {
  settings.mirror = cartridge["mirror/mode"].data == "horizontal";
}

};