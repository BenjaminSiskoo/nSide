struct KonamiVRC3 : Board {
  KonamiVRC3(Markup::Node& boardNode) : Board(boardNode), vrc3(*this) {
    settings.mirror = boardNode["mirror/mode"].text() == "vertical" ? 1 : 0;
  }

  auto main() -> void {
    vrc3.main();
  }

  auto prgRead(uint addr) -> uint8 {
    if((addr & 0xe000) == 0x6000) return read(prgram, addr & 0x1fff);
    if(addr & 0x8000) return read(prgrom, vrc3.prgAddress(addr));
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) return write(prgram, addr & 0x1fff, data);
    if(addr & 0x8000) return vrc3.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciramRead(addr);
    }
    return Board::chrRead(addr);
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciramWrite(addr, data);
    }
    return Board::chrWrite(addr, data);
  }

  auto power() -> void {
    vrc3.power();
  }

  auto reset() -> void {
    vrc3.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc3.serialize(s);
  }

  struct Settings {
    bool mirror;  //0 = horizontal, 1 = vertical
  } settings;

  VRC3 vrc3;
};
