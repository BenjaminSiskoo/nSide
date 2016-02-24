struct KonamiVRC7 : Board {
  KonamiVRC7(Markup::Node& board_node) : Board(board_node), vrc7(*this) {
    settings.pinout.a0 = 1 << board_node["chip/pinout/a0"].natural();
    settings.pinout.a1 = 1 << board_node["chip/pinout/a1"].natural();
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr < 0x6000) return cpu.mdr();
    if(addr < 0x8000) return read(prgram, addr);
    return read(prgrom, vrc7.prg_addr(addr));
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr < 0x6000) return;
    if(addr < 0x8000) return write(prgram, addr, data);

    bool a0 = (addr & settings.pinout.a0);
    bool a1 = (addr & settings.pinout.a1);
    addr &= 0xf000;
    addr |= (a1 << 1) | (a0 << 0);
    return vrc7.reg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciram_read(vrc7.ciram_addr(addr));
    return Board::chr_read(vrc7.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciram_write(vrc7.ciram_addr(addr), data);
    return Board::chr_write(vrc7.chr_addr(addr), data);
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
