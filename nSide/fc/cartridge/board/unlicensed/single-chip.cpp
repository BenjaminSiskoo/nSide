struct SingleChip : Board {
  SingleChip(Markup::Node& boardNode) : Board(boardNode) {
    settings.va10 = boardNode["va10"].natural();
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, addr);
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
  }

  auto chrRead(uint addr) -> uint8 {
    addr = ((addr & (1 << settings.va10)) >> (settings.va10 - 10)) | (addr & 0x03ff);
    return ppu.ciramRead(addr);
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    addr = ((addr & (1 << settings.va10)) >> (settings.va10 - 10)) | (addr & 0x03ff);
    return ppu.ciramWrite(addr, data);
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
  }

  struct Settings {
    uint4 va10;  //10 = vertical, 11 = horizontal, 12 = BLK0, 13 = BLK1
  } settings;
};
