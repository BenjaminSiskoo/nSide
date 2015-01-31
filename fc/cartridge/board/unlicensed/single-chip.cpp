struct SingleChip : Board {

struct Settings {
  uint4 va10;  //10 = vertical, 11 = horizontal, 12 = BLK0, 13 = BLK1
} settings;

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return prgrom.read(addr);
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
}

uint8 chr_read(unsigned addr) {
  addr = ((addr & (1 << settings.va10)) >> (settings.va10 - 10)) | (addr & 0x03ff);
  return ppu.ciram_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  addr = ((addr & (1 << settings.va10)) >> (settings.va10 - 10)) | (addr & 0x03ff);
  return ppu.ciram_write(addr, data);
}

void serialize(serializer& s) {
  Board::serialize(s);
}

SingleChip(Markup::Node& cartridge) : Board(cartridge) {
  settings.va10 = numeral(cartridge["board/va10"].data);
}

};
