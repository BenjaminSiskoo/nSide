struct KonamiVRC6 : Board {
  KonamiVRC6(Markup::Node& boardNode) : Board(boardNode), vrc6(*this) {
    settings.pinout.a0 = 1 << boardNode["chip/pinout/a0"].natural();
    settings.pinout.a1 = 1 << boardNode["chip/pinout/a1"].natural();
  }

  auto readPRG(uint addr, uint8 data) -> uint8 {
    if(addr < 0x6000) return data;
    if(addr < 0x8000) return vrc6.ramRead(addr, data);
    return read(prgrom, vrc6.prgAddress(addr));
  }

  auto writePRG(uint addr, uint8 data) -> void {
    if(addr < 0x6000) return;
    if(addr < 0x8000) return vrc6.ramWrite(addr, data);

    bool a0 = (addr & settings.pinout.a0);
    bool a1 = (addr & settings.pinout.a1);
    addr &= 0xf000;
    addr |= (a1 << 1) | (a0 << 0);
    return vrc6.regWrite(addr, data);
  }

  auto readCHR(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.readCIRAM(vrc6.ciramAddress(addr));
    return Board::readCHR(vrc6.chrAddress(addr));
  }

  auto writeCHR(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.writeCIRAM(vrc6.ciramAddress(addr), data);
    return Board::writeCHR(vrc6.chrAddress(addr), data);
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc6.serialize(s);
  }

  auto main() -> void { vrc6.main(); }
  auto power() -> void { vrc6.power(); }
  auto reset() -> void { vrc6.reset(); }

  struct Settings {
    struct Pinout {
      uint a0;
      uint a1;
    } pinout;
  } settings;

  VRC6 vrc6;
};
