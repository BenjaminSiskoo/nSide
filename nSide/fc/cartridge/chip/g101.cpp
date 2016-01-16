struct G101 : Chip {
  G101(Board& board, Markup::Node& board_node) : Chip(board) {
  }

  auto prg_addr(uint addr) const -> uint {
    switch(addr & 0xe000) {
    case 0x8000:
      if(prg_mode == 1) return (0x1e << 13) | (addr & 0x1fff);
      return (prg_bank[0] << 13) | (addr & 0x1fff);
    case 0xa000:
      return (prg_bank[1] << 13) | (addr & 0x1fff);
    case 0xc000:
      if(prg_mode == 0) return (0x1e << 13) | (addr & 0x1fff);
      return (prg_bank[0] << 13) | (addr & 0x1fff);
    case 0xe000:
      return (0x1f << 13) | (addr & 0x1fff);
    }
  }

  auto chr_addr(uint addr) const -> uint {
    return (chr_bank[addr >> 10] << 10) | (addr & 0x03ff);
  }

  auto ciram_addr(uint addr) const -> uint {
    switch(mirror) {
    case 0: return (addr & 0x03ff) | ((addr & 0x0400) >> 0);
    case 1: return (addr & 0x03ff) | ((addr & 0x0800) >> 1);
    }
  }

  auto reg_write(uint addr, uint8 data) -> void {
    switch(addr & 0xf000) {
    case 0x8000:
      prg_bank[0] = data & 0x1f;
      break;
    case 0x9000:
      mirror = data & 0x01;
      prg_mode = data & 0x02;
      break;
    case 0xa000:
      prg_bank[1] = data & 0x1f;
      break;
    case 0xb000:
      chr_bank[addr & 0x0007] = data;
      break;
    }
  }

  auto power() -> void {
    prg_mode = 0;
    prg_bank[0] = 0x00;
    prg_bank[1] = 0x1e;
    chr_bank[0] = 0;
    chr_bank[1] = 0;
    chr_bank[2] = 0;
    chr_bank[3] = 0;
    chr_bank[4] = 0;
    chr_bank[5] = 0;
    chr_bank[6] = 0;
    chr_bank[7] = 0;
    mirror = 0;
  }

  auto reset() -> void {
  }

  auto serialize(serializer& s) -> void {
    s.integer(prg_mode);
    s.array(prg_bank);
    s.array(chr_bank);
    s.integer(mirror);
  }

  bool prg_mode;
  uint5 prg_bank[2];
  uint8 chr_bank[8];
  bool mirror;
};
