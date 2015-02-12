//JALECO-JF-24
//JALECO-JF-25

struct JalecoJF2x : Board {

enum class Revision : unsigned {
  JF_24,
  JF_25,
} revision;

SS88006 ss88006;

void enter() {
  ss88006.enter();
}

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return prgrom.read(ss88006.prg_addr(addr));
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) return ss88006.reg_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ppu.ciram_read(ss88006.ciram_addr(addr));
  return Board::chr_read(ss88006.chr_addr(addr));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ppu.ciram_write(ss88006.ciram_addr(addr), data);
  return Board::chr_write(ss88006.chr_addr(addr), data);
}

void power() {
  ss88006.power();
}

void reset() {
  ss88006.reset();
}

void serialize(serializer& s) {
  Board::serialize(s);
  ss88006.serialize(s);
}

JalecoJF2x(Markup::Node& cartridge) : Board(cartridge), ss88006(*this, cartridge) {
  string type = cartridge["board/type"].data;
  if(type.match("*JF-24*" )) revision = Revision::JF_24;
  if(type.match("*JF-25*" )) revision = Revision::JF_25;
}

};
