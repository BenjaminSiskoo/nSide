struct KonamiVRC6 : Board {
  KonamiVRC6(Markup::Node& board_node) : Board(board_node), vrc6(*this) {
    settings.pinout.a0 = 1 << board_node["chip/pinout/a0"].natural();
    settings.pinout.a1 = 1 << board_node["chip/pinout/a1"].natural();
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr < 0x6000) return cpu.mdr();
    if(addr < 0x8000) return vrc6.ram_read(addr);
    return read(prgrom, vrc6.prg_addr(addr));
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr < 0x6000) return;
    if(addr < 0x8000) return vrc6.ram_write(addr, data);

    bool a0 = (addr & settings.pinout.a0);
    bool a1 = (addr & settings.pinout.a1);
    addr &= 0xf000;
    addr |= (a1 << 1) | (a0 << 0);
    return vrc6.reg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciram_read(vrc6.ciram_addr(addr));
    return Board::chr_read(vrc6.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciram_write(vrc6.ciram_addr(addr), data);
    return Board::chr_write(vrc6.chr_addr(addr), data);
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    vrc6.serialize(s);
  }

  auto enter() -> void { vrc6.enter(); }
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
