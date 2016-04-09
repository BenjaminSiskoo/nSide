struct SingleChip : Board {
  SingleChip(Markup::Node& board_node) : Board(board_node) {
    settings.va10 = board_node["va10"].natural();
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, addr);
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
  }

  auto chr_read(uint addr) -> uint8 {
    addr = ((addr & (1 << settings.va10)) >> (settings.va10 - 10)) | (addr & 0x03ff);
    return ppu.ciramRead(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
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
