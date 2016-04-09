struct IremH3001 : Board {
  IremH3001(Markup::Node& board_node) : Board(board_node), ifh3001(*this, board_node) {
  }

  auto main() -> void {
    ifh3001.main();
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x8000) return read(prgrom, ifh3001.prg_addr(addr));
    if((addr & 0xe000) == 0x6000) return read(prgram, addr & 0x1fff);
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0x8000) == 0x8000) return ifh3001.reg_write(addr, data);
    if((addr & 0xe000) == 0x6000) return write(prgram, addr & 0x1fff, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(ifh3001.ciram_addr(addr));
    return Board::chr_read(ifh3001.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(ifh3001.ciram_addr(addr), data);
    return Board::chr_write(ifh3001.chr_addr(addr), data);
  }

  auto power() -> void {
    ifh3001.power();
  }

  auto reset() -> void {
    ifh3001.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    ifh3001.serialize(s);
  }

  IFH3001 ifh3001;
};
