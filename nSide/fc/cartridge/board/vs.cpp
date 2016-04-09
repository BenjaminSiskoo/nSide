//VS

struct VS : Board {
  VS(Markup::Node& board_node) : Board(board_node),
  mmc1(*this, board_node),
  n108(*this, board_node) {
    chip_type = ChipType::None;
    string type = board_node["chip/type"].text();
    if(type.match("74HC32")) chip_type = ChipType::_74HC32;
    if(type.match("MMC1*" )) chip_type = ChipType::MMC1;
    if(type.match("108"   )) chip_type = ChipType::N108;
    if(type.match("109"   )) chip_type = ChipType::N108;
    if(type.match("118"   )) chip_type = ChipType::N108;
    if(type.match("119"   )) chip_type = ChipType::N108;
  }

  auto main() -> void {
    if(chip_type == ChipType::MMC1) return mmc1.main();
    tick();
  }

  auto prg_read(uint addr) -> uint8 {
    switch(chip_type) {
    case ChipType::None:
      if(addr & 0x8000) {
        if(addr < 0xe000 && prgrom.size() < 0x2000) return cpu.mdr();
        if(addr < 0xc000 && prgrom.size() < 0x4000) return cpu.mdr();
        if(addr < 0xa000 && prgrom.size() < 0x6000) return cpu.mdr();
        addr &= 0x7fff;
        if(prgrom.size() > 0x8000) { // Games with oversize 1D such as VS. Gumshoe
          if(addr >= 0x2000 || bank == 1) addr += 0x2000;
        }
        return read(prgrom, addr);
      }
      break;
    case ChipType::_74HC32:
      if(addr & 0x8000) {
        if((addr & 0xc000) == 0x8000)
          return read(prgrom, (bank << 14) | (addr & 0x3fff));
        else
          return read(prgrom, (0x0f << 14) | (addr & 0x3fff));
      }
      break;
    case ChipType::MMC1:
      if(addr & 0x8000) return read(prgrom, mmc1.prg_addr(addr));
      //if(revision == Revision::SUROM || revision == Revision::SXROM) {
      //  addr |= ((mmc1.chr_bank[last_chr_bank] & 0x10) >> 4) << 18;
      //}
      break;
    case ChipType::N108:
      if(addr & 0x8000) return read(prgrom, n108.prg_addr(addr));
      break;
    }
    if((addr & 0xe000) == 0x6000) {
      if(prgram.size() == 0) return vssystem.read(cpu.side, addr, cpu.mdr());
      else                 return read(prgram, addr);
    }
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    switch(chip_type) {
    case ChipType::None:
      if(addr == 0x4016) bank = (data & 0x04) >> 2;
      break;
    case ChipType::_74HC32:
      //TODO: Check if VS. UNROM has bus conflicts
      //data &= prg_read(addr);
      if(addr & 0x8000) bank = data & 0x0f;
      break;
    case ChipType::MMC1:
      if(addr & 0x8000) return mmc1.mmio_write(addr, data);
      break;
    case ChipType::N108:
      if(addr & 0x8000) return n108.reg_write(addr, data);
      break;
    }
    if((addr & 0xe000) == 0x6000) {
      if(prgram.size() == 0) vssystem.write(cpu.side, addr, data);
      else                   write(prgram, addr, data);
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(addr);
    switch(chip_type) {
    case ChipType::None:
      if(chrrom.size() < bank << 13) return ppu.status.mdr;
      return read(chrrom, ((bank << 13) + (addr & 0x1fff)));
    case ChipType::_74HC32:
      return Board::chr_read(addr);
    case ChipType::MMC1:
      return Board::chr_read(mmc1.chr_addr(addr));
    case ChipType::N108:
      return Board::chr_read(n108.chr_addr(addr));
    }
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(addr, data);
    switch(chip_type) {
    case ChipType::None:
      break;
    case ChipType::_74HC32:
      return Board::chr_write(addr, data);
    case ChipType::MMC1:
      return Board::chr_write(mmc1.chr_addr(addr), data);
    case ChipType::N108:
      return Board::chr_write(n108.chr_addr(addr), data);
    }
  }

  auto power() -> void {
    switch(chip_type) {
    case ChipType::MMC1: mmc1.power(); break;
    case ChipType::N108: n108.power(); break;
    }
  }

  auto reset() -> void {
    bank = 0;
    switch(chip_type) {
    case ChipType::MMC1: mmc1.reset(); break;
    case ChipType::N108: n108.reset(); break;
    }
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(bank);
    switch(chip_type) {
    case ChipType::MMC1: mmc1.serialize(s); break;
    case ChipType::N108: n108.serialize(s); break;
    }
  }

  enum class ChipType : uint {
    None,
    _74HC32,
    MMC1,
    N108,
  } chip_type;

  uint4 bank;
  MMC1 mmc1;
  N108 n108;
};
