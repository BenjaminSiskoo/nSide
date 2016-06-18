struct KonamiVRC1 : Board {
  KonamiVRC1(Markup::Node& boardNode) : Board(boardNode), vrc1(*this) {
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, vrc1.prgAddress(addr));
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if(addr & 0x8000) return vrc1.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(vrc1.ciramAddress(addr));
    return Board::chrRead(vrc1.chrAddress(addr));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(vrc1.ciramAddress(addr), data);
    return Board::chrWrite(vrc1.chrAddress(addr), data);
  }

  auto power() -> void {
    vrc1.power();
  }

  auto reset() -> void {
    vrc1.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc1.serialize(s);
  }

  VRC1 vrc1;
};
