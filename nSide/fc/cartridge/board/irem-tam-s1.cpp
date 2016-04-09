//IREM-TAM-S1

struct IremTamS1 : Board {
  IremTamS1(Markup::Node& board_node) : Board(board_node) {
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x0000) return cpu.mdr();
    switch(addr & 0xc000) {
    case 0x8000: return read(prgrom, (    0x0f << 14) | (addr & 0x3fff));
    case 0xc000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
    }
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      //TODO: check for bus conflicts
      //data &= prg_read(addr);
      prg_bank = data & 0x0f;
      mirror = (data & 0xc0) >> 6;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      switch(mirror) {
      case 0: addr = (0x0000              ) | (addr & 0x03ff); break;
      case 1: addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff); break;
      case 2: addr = ((addr & 0x0400) >> 0) | (addr & 0x03ff); break;
      case 3: addr = (0x0400              ) | (addr & 0x03ff); break;
      }
      return ppu.ciramRead(addr);
    }
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      switch(mirror) {
      case 0: addr = (0x0000              ) | (addr & 0x03ff); break;
      case 1: addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff); break;
      case 2: addr = ((addr & 0x0400) >> 0) | (addr & 0x03ff); break;
      case 3: addr = (0x0400              ) | (addr & 0x03ff); break;
      }
      return ppu.ciramWrite(addr, data);
    }
    return Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
    mirror = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);

    s.integer(prg_bank);
    s.integer(mirror);
  }

  uint4 prg_bank;
  uint2 mirror; //0 = screen 0, 1 = horizontal, 2 = vertical, 3 = screen 1
};
