  //BANDAI-74*161/02/74

struct Bandai74_161_02_74 : Board {
  Bandai74_161_02_74(Markup::Node& boardNode) : Board(boardNode) {
  }

  auto main() -> void {
    chrAddressBusTest(ppu.status.chrAddressBus);
    tick();
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prgBank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      data &= prgRead(addr);
      prgBank = (data & 0x03) >> 0;
      chrPlane = (data & 0x04) >> 2;
    }
  }

  auto chrAddress(uint addr) -> uint {
    switch(addr & 0x1000) {
    case 0x0000: return (addr & 0x0fff) | (chrPlane << 14) | (chrBank << 12);
    case 0x1000: return (addr & 0x0fff) | (chrPlane << 14);
    }
  }

  auto chrRead(uint addr) -> uint8 {
    chrAddressBusTest(addr);
    if(addr & 0x2000) return ppu.ciramRead(addr & 0x07ff);
    return Board::chrRead(chrAddress(addr));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    chrAddressBusTest(addr);
    if(addr & 0x2000) return ppu.ciramWrite(addr & 0x07ff, data);
    Board::chrWrite(chrAddress(addr), data);
  }

  auto chrAddressBusTest(uint addr) -> void {
    if((addr & 0x3000) != (chrAddressBus & 0x3000) && (addr & 0x3000) == 0x2000) {
      // NOR logic reverses bank numbers
      chrBank = (((addr & 0x0300) ^ 0x0300) >> 8);
    }
    chrAddressBus = addr & 0x3000;
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prgBank = 0;
    chrPlane = 0;
    chrBank = 0;
    chrAddressBus = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prgBank);
    s.integer(chrPlane);
    s.integer(chrBank);
    s.integer(chrAddressBus);
  }

  uint2 prgBank;
  bool chrPlane;
  uint2 chrBank;
  uint16 chrAddressBus;
};
