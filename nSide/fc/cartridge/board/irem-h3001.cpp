struct IremH3001 : Board {
  IremH3001(Markup::Node& boardNode) : Board(boardNode), ifh3001(*this, boardNode) {
  }

  auto main() -> void {
    ifh3001.main();
  }

  auto prgRead(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x8000) return read(prgrom, ifh3001.prgAddress(addr));
    if((addr & 0xe000) == 0x6000) return read(prgram, addr & 0x1fff);
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if((addr & 0x8000) == 0x8000) return ifh3001.regWrite(addr, data);
    if((addr & 0xe000) == 0x6000) return write(prgram, addr & 0x1fff, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(ifh3001.ciramAddress(addr));
    return Board::chrRead(ifh3001.chrAddress(addr));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(ifh3001.ciramAddress(addr), data);
    return Board::chrWrite(ifh3001.chrAddress(addr), data);
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
