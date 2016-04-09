  //BANDAI-74*161/02/74

struct Bandai74_161_02_74 : Board {
  Bandai74_161_02_74(Markup::Node& board_node) : Board(board_node) {
  }

  auto main() -> void {
    chr_abus_test(ppu.status.chr_abus);
    tick();
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      data &= prg_read(addr);
      prg_bank = (data & 0x03) >> 0;
      chr_plane = (data & 0x04) >> 2;
    }
  }

  auto chr_addr(uint addr) -> uint {
    switch(addr & 0x1000) {
    case 0x0000: return (addr & 0x0fff) | (chr_plane << 14) | (chr_bank << 12);
    case 0x1000: return (addr & 0x0fff) | (chr_plane << 14);
    }
  }

  auto chr_read(uint addr) -> uint8 {
    chr_abus_test(addr);
    if(addr & 0x2000) return ppu.ciramRead(addr & 0x07ff);
    return Board::chr_read(chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    chr_abus_test(addr);
    if(addr & 0x2000) return ppu.ciramWrite(addr & 0x07ff, data);
    Board::chr_write(chr_addr(addr), data);
  }

  auto chr_abus_test(uint addr) -> void {
    if((addr & 0x3000) != (chr_abus & 0x3000) && (addr & 0x3000) == 0x2000) {
      // NOR logic reverses bank numbers
      chr_bank = (((addr & 0x0300) ^ 0x0300) >> 8);
    }
    chr_abus = addr & 0x3000;
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
    chr_plane = 0;
    chr_bank = 0;
    chr_abus = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prg_bank);
    s.integer(chr_plane);
    s.integer(chr_bank);
    s.integer(chr_abus);
  }

  uint2 prg_bank;
  bool chr_plane;
  uint2 chr_bank;
  uint16 chr_abus;
};
