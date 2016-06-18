struct TaitoTC : Board {
  TaitoTC(Markup::Node& boardNode) : Board(boardNode), tc(*this, boardNode) {
  }

  auto main() -> void {
    tc.main();
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, tc.prgAddress(addr));
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if(addr & 0x8000) return tc.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(tc.ciramAddress(addr));
    return Board::chrRead(tc.chrAddress(addr));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(tc.ciramAddress(addr), data);
    return Board::chrWrite(tc.chrAddress(addr), data);
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
