struct IremG101 : Board {

struct Settings {
  uint2 mirror;  //0 = G101-controlled, 1 = screen 0, 2 = screen 1
} settings;

G101 g101;

uint8 prg_read(unsigned addr) {
  if((addr & 0x8000) == 0x8000) return read(prgrom, g101.prg_addr(addr));
  if((addr & 0xe000) == 0x6000) return read(prgram, addr & 0x1fff);
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if((addr & 0x8000) == 0x8000) return g101.reg_write(addr, data);
  if((addr & 0xe000) == 0x6000) return write(prgram, addr & 0x1fff, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) switch(settings.mirror) {
  case 0: return ppu.ciram_read(g101.ciram_addr(addr));
  case 1: return ppu.ciram_read((addr & 0x03ff) | 0x0400);
  case 2: return ppu.ciram_read((addr & 0x03ff) | 0x0800);
  }
  return Board::chr_read(g101.chr_addr(addr));
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) switch(settings.mirror) {
  case 0: return ppu.ciram_write(g101.ciram_addr(addr), data);
  case 1: return ppu.ciram_write((addr & 0x03ff) | 0x0400, data);
  case 2: return ppu.ciram_write((addr & 0x03ff) | 0x0800, data);
  }
  return Board::chr_write(g101.chr_addr(addr), data);
}

void power() {
  g101.power();
}

void reset() {
  g101.reset();
}

void serialize(serializer& s) {
  Board::serialize(s);
  g101.serialize(s);
}

IremG101(Markup::Node& cartridge) : Board(cartridge), g101(*this, cartridge) {
  if(!cartridge["mirror"]) {
    settings.mirror = 0;
  } else {
    string mirror = cartridge["mirror/mode"].text();
    if(mirror == "screen-0") settings.mirror = 1;
    if(mirror == "screen-1") settings.mirror = 2;
  }
}

};
