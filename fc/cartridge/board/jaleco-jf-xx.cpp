//JALECO-JF-05
//JALECO-JF-06
//JALECO-JF-07
//JALECO-JF-08
//JALECO-JF-09
//JALECO-JF-10
//JALECO-JF-11
//JALECO-JF-14

struct JalecoJFxx : Board {

enum class Revision : unsigned {
  JF_05,
  JF_06,
  JF_07,
  JF_08,
  JF_09,
  JF_10,
  JF_11,
  JF_14,
} revision;

struct Settings {
  bool mirror;  //0 = vertical, 1 = horizontal
} settings;

uint2 prg_bank;
uint4 chr_bank;

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return prgrom.read((prg_bank << 15) | (addr & 0x7fff));
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if((addr & 0xe000) == 0x6000) {
    switch(revision) {
    case Revision::JF_05:
    case Revision::JF_06:
    case Revision::JF_07:
    case Revision::JF_08:
    case Revision::JF_09:
    case Revision::JF_10:
      chr_bank = ((data & 0x01) << 1) | ((data & 0x02) >> 1);
      break;
    case Revision::JF_11:
    case Revision::JF_14:
      prg_bank = (data & 0x30) >> 4;
      chr_bank = (data & 0x0f) >> 0;
      break;
    }
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

JalecoJFxx(Markup::Node& cartridge) : Board(cartridge) {
  string type = cartridge["board/type"].data;
  if(type.match("*JF-05*" )) revision = Revision::JF_05;
  if(type.match("*JF-06*" )) revision = Revision::JF_06;
  if(type.match("*JF-07*" )) revision = Revision::JF_07;
  if(type.match("*JF-08*" )) revision = Revision::JF_08;
  if(type.match("*JF-09*" )) revision = Revision::JF_09;
  if(type.match("*JF-10*" )) revision = Revision::JF_10;
  if(type.match("*JF-11*" )) revision = Revision::JF_11;
  if(type.match("*JF-14*" )) revision = Revision::JF_14;

  settings.mirror = cartridge["mirror/mode"].data == "horizontal";
}

};
