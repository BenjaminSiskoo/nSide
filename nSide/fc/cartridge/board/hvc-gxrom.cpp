//NES-GNROM
//NES-MHROM

struct HVC_GxROM : Board {
  HVC_GxROM(Markup::Node& boardNode) : Board(boardNode) {
    settings.mirror = boardNode["mirror/mode"].text() == "horizontal";
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prgBank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      // Bus conflicts
      data &= prgRead(addr);
      prgBank = (data & 0x30) >> 4;
      chrBank = (data & 0x03) >> 0;
    }
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciramRead(addr);
    }
    addr = (chrBank * 0x2000) + (addr & 0x1fff);
    return Board::chrRead(addr);
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciramWrite(addr, data);
    }
    addr = (chrBank * 0x2000) + (addr & 0x1fff);
    Board::chrWrite(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prgBank = 0;
    chrBank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prgBank);
    s.integer(chrBank);
  }

  struct Settings {
    bool mirror;  //0 = vertical, 1 = horizontal
  } settings;

  uint2 prgBank;
  uint2 chrBank;
};
