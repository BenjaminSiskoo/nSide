//SUNSOFT-4

struct Sunsoft4 : Board {
  Sunsoft4(Markup::Node& board_node) : Board(board_node) {
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0xc000) == 0x8000) return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
    if((addr & 0xc000) == 0xc000) return read(prgrom, (    0xff << 14) | (addr & 0x3fff));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    switch(addr & 0xf000) {
    case 0x8000: chr_bank[0] = data; break;
    case 0x9000: chr_bank[1] = data; break;
    case 0xa000: chr_bank[2] = data; break;
    case 0xb000: chr_bank[3] = data; break;
    case 0xc000: ntrom_bank[0] = data | 0x80; break;
    case 0xd000: ntrom_bank[1] = data | 0x80; break;
    case 0xe000:
      mirror = data & 0x03;
      nametable_mode = data & 0x10;
      break;
    case 0xf000: prg_bank = data; break;
    }
  }

  auto ciram_addr(uint addr) -> uint {
    switch(mirror) {
    case 0: return ((addr & 0x0400) >> 0) | (addr & 0x03ff);
    case 1: return ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    case 2: return 0x0000 | (addr & 0x03ff);
    case 3: return 0x0400 | (addr & 0x03ff);
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      addr = ciram_addr(addr);
      if(nametable_mode) return Board::chr_read(((ntrom_bank[(addr & 0x0400) >> 10] | 0x80) << 10) | (addr & 0x03FF));
      else               return ppu.ciramRead(addr);
    }
    addr = (chr_bank[(addr & 0x1800) >> 11] << 11) | (addr & 0x07ff);
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(nametable_mode == 0) return ppu.ciramWrite(ciram_addr(addr), data);
    } else {
      return Board::chr_write(addr, data);
    }
  }

  auto power() -> void {
  }

  auto reset() -> void {
    chr_bank[0] = 0;
    chr_bank[1] = 0;
    chr_bank[2] = 0;
    chr_bank[3] = 0;
    ntrom_bank[0] = 0;
    ntrom_bank[1] = 0;
    mirror = 0;
    nametable_mode = 0;
    prg_bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.array(chr_bank);
    s.array(ntrom_bank);
    s.integer(mirror);
    s.integer(nametable_mode);
    s.integer(prg_bank);
  }

  uint8 chr_bank[4];
  uint7 ntrom_bank[2];
  uint2 mirror;
  bool nametable_mode;
  uint8 prg_bank;
};
