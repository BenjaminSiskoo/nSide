//VS

struct VS : Board {
  VS(Markup::Node& boardNode) : Board(boardNode),
  mmc1(*this, boardNode),
  n108(*this, boardNode) {
    chipType = ChipType::None;
    string type = boardNode["chip/type"].text();
    if(type.match("74HC32")) chipType = ChipType::_74HC32;
    if(type.match("MMC1*" )) chipType = ChipType::MMC1;
    if(type.match("108"   )) chipType = ChipType::N108;
    if(type.match("109"   )) chipType = ChipType::N108;
    if(type.match("118"   )) chipType = ChipType::N108;
    if(type.match("119"   )) chipType = ChipType::N108;
  }

  auto main() -> void {
    if(chipType == ChipType::MMC1) return mmc1.main();
    tick();
  }

  auto prgRead(uint addr) -> uint8 {
    switch(chipType) {
    case ChipType::None: {
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
    }

    case ChipType::_74HC32: {
      if(addr & 0x8000) {
        if((addr & 0xc000) == 0x8000)
          return read(prgrom, (bank << 14) | (addr & 0x3fff));
        else
          return read(prgrom, (0x0f << 14) | (addr & 0x3fff));
      }
      break;
    }

    case ChipType::MMC1: {
      if(addr & 0x8000) return read(prgrom, mmc1.prgAddress(addr));
      //if(revision == Revision::SUROM || revision == Revision::SXROM) {
      //  addr |= ((mmc1.chrBank[lastCHRBank] & 0x10) >> 4) << 18;
      //}
      break;
    }

    case ChipType::N108: {
      if(addr & 0x8000) return read(prgrom, n108.prgAddress(addr));
      break;
    }

    }
    if((addr & 0xe000) == 0x6000) {
      if(prgram.size() == 0) return vssystem.read(cpu.side, addr, cpu.mdr());
      else                   return read(prgram, addr);
    }
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    switch(chipType) {
    case ChipType::None:
      if(addr == 0x4016) bank = (data & 0x04) >> 2;
      break;
    case ChipType::_74HC32:
      //TODO: Check if VS. UNROM has bus conflicts
      //data &= prgRead(addr);
      if(addr & 0x8000) bank = data & 0x0f;
      break;
    case ChipType::MMC1:
      if(addr & 0x8000) return mmc1.mmioWrite(addr, data);
      break;
    case ChipType::N108:
      if(addr & 0x8000) return n108.regWrite(addr, data);
      break;
    }
    if((addr & 0xe000) == 0x6000) {
      if(prgram.size() == 0) vssystem.write(cpu.side, addr, data);
      else                   write(prgram, addr, data);
    }
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(addr);
    switch(chipType) {
    case ChipType::None:
      if(chrrom.size() < bank << 13) return ppu.status.mdr;
      return read(chrrom, ((bank << 13) + (addr & 0x1fff)));
    case ChipType::_74HC32:
      return Board::chrRead(addr);
    case ChipType::MMC1:
      return Board::chrRead(mmc1.chrAddress(addr));
    case ChipType::N108:
      return Board::chrRead(n108.chrAddress(addr));
    }
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(addr, data);
    switch(chipType) {
    case ChipType::None:
      break;
    case ChipType::_74HC32:
      return Board::chrWrite(addr, data);
    case ChipType::MMC1:
      return Board::chrWrite(mmc1.chrAddress(addr), data);
    case ChipType::N108:
      return Board::chrWrite(n108.chrAddress(addr), data);
    }
  }

  auto power() -> void {
    switch(chipType) {
    case ChipType::MMC1: mmc1.power(); break;
    case ChipType::N108: n108.power(); break;
    }
  }

  auto reset() -> void {
    bank = 0;
    switch(chipType) {
    case ChipType::MMC1: mmc1.reset(); break;
    case ChipType::N108: n108.reset(); break;
    }
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(bank);
    switch(chipType) {
    case ChipType::MMC1: mmc1.serialize(s); break;
    case ChipType::N108: n108.serialize(s); break;
    }
  }

  enum class ChipType : uint {
    None,
    _74HC32,
    MMC1,
    N108,
  } chipType;

  uint4 bank;
  MMC1 mmc1;
  N108 n108;
};
