//IREM-HOLYDIVER

struct IremHolyDiver : Board {
  IremHolyDiver(Markup::Node& board_node) : Board(board_node) {
  }

  auto prg_read(uint addr) -> uint8 {
    switch(addr & 0xc000) {
    case 0x8000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
    case 0xc000: return read(prgrom, (    0x07 << 14) | (addr & 0x3fff));
    }
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      // Bus conflicts
      data &= prg_read(addr);
      prg_bank = (data & 0x07) >> 0;
      mirror = data & 0x08;
      chr_bank = (data & 0xf0) >> 4;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr);
    }
    return Board::chr_read((chr_bank * 0x2000) + (addr & 0x1fff));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(mirror == 0) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr, data);
    }
    Board::chr_write((chr_bank * 0x2000) + (addr & 0x1fff), data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
    chr_bank = 0;
    mirror = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prg_bank);
    s.integer(chr_bank);
    s.integer(mirror);
  }

  uint3 prg_bank;
  uint4 chr_bank;
  bool mirror;
};
