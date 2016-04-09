struct IremG101 : Board {
  IremG101(Markup::Node& board_node) : Board(board_node), g101(*this, board_node) {
    if(!board_node["mirror"]) {
      settings.mirror = 0;
    } else {
      string mirror = board_node["mirror/mode"].text();
      if(mirror == "screen-0") settings.mirror = 1;
      if(mirror == "screen-1") settings.mirror = 2;
    }
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x8000) return read(prgrom, g101.prg_addr(addr));
    if((addr & 0xe000) == 0x6000) return read(prgram, addr & 0x1fff);
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0x8000) == 0x8000) return g101.reg_write(addr, data);
    if((addr & 0xe000) == 0x6000) return write(prgram, addr & 0x1fff, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) switch(settings.mirror) {
    case 0: return ppu.ciramRead(g101.ciram_addr(addr));
    case 1: return ppu.ciramRead((addr & 0x03ff) | 0x0400);
    case 2: return ppu.ciramRead((addr & 0x03ff) | 0x0800);
    }
    return Board::chr_read(g101.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) switch(settings.mirror) {
    case 0: return ppu.ciramWrite(g101.ciram_addr(addr), data);
    case 1: return ppu.ciramWrite((addr & 0x03ff) | 0x0400, data);
    case 2: return ppu.ciramWrite((addr & 0x03ff) | 0x0800, data);
    }
    return Board::chr_write(g101.chr_addr(addr), data);
  }

  auto power() -> void {
    g101.power();
  }

  auto reset() -> void {
    g101.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    g101.serialize(s);
  }

  struct Settings {
    uint2 mirror;  //0 = G101-controlled, 1 = screen 0, 2 = screen 1
  } settings;

  G101 g101;
};
