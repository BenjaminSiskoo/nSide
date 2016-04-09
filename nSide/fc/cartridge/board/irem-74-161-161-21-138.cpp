//IREM-74*161/161/21/138

struct Irem74_161_161_21_138 : Board {
  Irem74_161_161_21_138(Markup::Node& board_node) : Board(board_node) {
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      // Bus conflicts
      data &= prg_read(addr);
      prg_bank = (data & 0x0f) >> 0;
      chr_bank = (data & 0xf0) >> 4;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ciramRead(addr);
    if((addr & 0x1800) == 0x0000) return read(chrrom, (addr & 0x07ff) | (chr_bank << 11));
    return read(chrram, addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ciramWrite(addr, data);
    if((addr & 0x1800) != 0x0000) return write(chrram, addr & 0x1fff, data);
  }

  auto ciramRead(uint addr) -> uint8 {
    switch(addr & 0x0800) {
    case 0x0000: return read(chrram, addr & 0x1fff);
    case 0x0800: return ppu.ciramRead(addr);
    }
  }

  auto ciramWrite(uint addr, uint8 data) -> void {
    switch(addr & 0x0800) {
    case 0x0000: return write(chrram, addr & 0x1fff, data);
    case 0x0800: return ppu.ciramWrite(addr, data);
    }
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
    chr_bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prg_bank);
    s.integer(chr_bank);
  }

  uint4 prg_bank;
  uint4 chr_bank;
};
