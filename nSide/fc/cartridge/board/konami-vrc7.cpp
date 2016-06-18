struct KonamiVRC7 : Board {
  KonamiVRC7(Markup::Node& boardNode) : Board(boardNode), vrc7(*this) {
    settings.pinout.a0 = 1 << boardNode["chip/pinout/a0"].natural();
    settings.pinout.a1 = 1 << boardNode["chip/pinout/a1"].natural();
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr < 0x6000) return cpu.mdr();
    if(addr < 0x8000) return read(prgram, addr);
    return read(prgrom, vrc7.prgAddress(addr));
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if(addr < 0x6000) return;
    if(addr < 0x8000) return write(prgram, addr, data);

    bool a0 = (addr & settings.pinout.a0);
    bool a1 = (addr & settings.pinout.a1);
    addr &= 0xf000;
    addr |= (a1 << 1) | (a0 << 0);
    return vrc7.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(vrc7.ciramAddress(addr));
    return Board::chrRead(vrc7.chrAddress(addr));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(vrc7.ciramAddress(addr), data);
    return Board::chrWrite(vrc7.chrAddress(addr), data);
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc7.serialize(s);
  }

  auto main() -> void { vrc7.main(); }
  auto power() -> void { vrc7.power(); }
  auto reset() -> void { vrc7.reset(); }

  struct Settings {
    struct Pinout {
      uint a0;
      uint a1;
    } pinout;
  } settings;

  VRC7 vrc7;
};
