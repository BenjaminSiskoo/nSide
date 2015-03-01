//NES-UNROM
//NES-UN1ROM
//NES-UOROM

struct NES_UxROM : Board {

enum class Revision : unsigned {
  UNROM,
  UN1ROM,
  UOROM,
} revision;

enum class ChipType : unsigned {
  _7432,
  _7408,
} chip_type;

struct Settings {
  bool mirror;    //0 = vertical, 1 = horizontal
} settings;

uint4 prg_bank;

uint8 prg_read(unsigned addr) {
  if((addr & 0x8000) == 0x0000) return cpu.mdr();
  switch(chip_type) {
  case ChipType::_7432:
    switch(addr & 0xc000) {
    case 0x8000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
    case 0xc000: return read(prgrom, (    0x0f << 14) | (addr & 0x3fff));
    }
  case ChipType::_7408:
    switch(addr & 0xc000) {
    case 0x8000: return read(prgrom, (    0x00 << 14) | (addr & 0x3fff));
    case 0xc000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
    }
  }
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    // Bus conflicts
    data &= prg_read(addr);
    if(revision != Revision::UN1ROM) prg_bank = data & 0x0f;
    if(revision == Revision::UN1ROM) prg_bank = (data & 0x1c) >> 2;
  }
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) {
    if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_read(addr);
  }
  return Board::chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) {
    if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    return ppu.ciram_write(addr, data);
  }
  return Board::chr_write(addr, data);
}

void power() {
}

void reset() {
  prg_bank = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);

  s.integer(prg_bank);
}

NES_UxROM(Markup::Node& cartridge) : Board(cartridge) {
  settings.mirror = cartridge["mirror/mode"].data == "horizontal";
  string type = cartridge["board/type"].data;
  if(type.match("*UNROM" )) revision = Revision::UNROM;
  if(type.match("*UN1ROM")) revision = Revision::UN1ROM;
  if(type.match("*UOROM" )) revision = Revision::UOROM;
  type = cartridge["chip/type"].data;
  if(type.match("74*32")) chip_type = ChipType::_7432;
  if(type.match("74*08")) chip_type = ChipType::_7408;
}

};
