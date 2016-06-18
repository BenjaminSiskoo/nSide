struct KonamiVRC2 : Board {
  KonamiVRC2(Markup::Node& boardNode) : Board(boardNode), vrc2(*this) {
    settings.pinout.a0 = 1 << boardNode["chip/pinout/a0"].natural();
    settings.pinout.a1 = 1 << boardNode["chip/pinout/a1"].natural();
    settings.pinout.chrShift = boardNode["chip/pinout/chr-shift"].natural();
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr < 0x6000) return cpu.mdr();
    if(addr < 0x8000) return vrc2.ramRead(addr);
    return read(prgrom, vrc2.prgAddress(addr));
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if(addr < 0x6000) return;
    if(addr < 0x8000) return vrc2.ramWrite(addr, data);

    bool a0 = (addr & settings.pinout.a0);
    bool a1 = (addr & settings.pinout.a1);
    addr &= 0xfff0;
    addr |= (a0 << 0) | (a1 << 1);
    return vrc2.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(vrc2.ciramAddress(addr));
    addr = vrc2.chrAddress(addr);
    addr = ((addr >> settings.pinout.chrShift) & ~0x3ff) | (addr & 0x3ff);
    return Board::chrRead(addr);
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(vrc2.ciramAddress(addr), data);
    return Board::chrWrite(vrc2.chrAddress(addr), data);
  }

  auto power() -> void {
    vrc2.power();
  }

  auto reset() -> void {
    vrc2.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc2.serialize(s);
  }

  struct Settings {
    struct Pinout {
      uint a0;
      uint a1;
      uint chrShift;
    } pinout;
  } settings;

  VRC2 vrc2;
};
