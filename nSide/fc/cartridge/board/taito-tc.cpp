struct TaitoTC : Board {
  TaitoTC(Markup::Node& board_node) : Board(board_node), tc(*this, board_node) {
  }

  auto main() -> void {
    tc.main();
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, tc.prg_addr(addr));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) return tc.reg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(tc.ciram_addr(addr));
    return Board::chr_read(tc.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(tc.ciram_addr(addr), data);
    return Board::chr_write(tc.chr_addr(addr), data);
  }

  auto power() -> void {
    tc.power();
  }

  auto reset() -> void {
    tc.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    tc.serialize(s);
  }

  TC tc;
};
