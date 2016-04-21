struct HVC_TxROM : Board {
  HVC_TxROM(Markup::Node& board_node) : Board(board_node), mmc3(*this, board_node) {
    string type = board_node["id"].text();
    if(type.match("*TBROM"  )) revision = Revision::TBROM;
    if(type.match("*TEROM"  )) revision = Revision::TEROM;
    if(type.match("*TFROM"  )) revision = Revision::TFROM;
    if(type.match("*TGROM"  )) revision = Revision::TGROM;
    if(type.match("*TKROM"  )) revision = Revision::TKROM;
    if(type.match("*TKEPROM")) revision = Revision::TKEPROM;
    if(type.match("*TKSROM" )) revision = Revision::TKSROM;
    if(type.match("*TLROM"  )) revision = Revision::TLROM;
    if(type.match("*TL1ROM" )) revision = Revision::TL1ROM;
    if(type.match("*TL2ROM" )) revision = Revision::TL2ROM;
    if(type.match("*TLSROM" )) revision = Revision::TLSROM;
    if(type.match("*TNROM"  )) revision = Revision::TNROM;
    if(type.match("*TQROM"  )) revision = Revision::TQROM;
    if(type.match("*TR1ROM" )) revision = Revision::TR1ROM;
    if(type.match("*TSROM"  )) revision = Revision::TSROM;
    if(type.match("*TVROM"  )) revision = Revision::TVROM;
    if(type.match("*MC-ACC" )) revision = Revision::MCACC;
  }

  auto main() -> void {
    mmc3.main();
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0xe000) == 0x6000 && prgram.size() > 0) return mmc3.ram_read(addr);
    if(addr & 0x8000) return read(prgrom, mmc3.prg_addr(addr));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000 && prgram.size() > 0) return mmc3.ram_write(addr, data);
    if(addr & 0x8000) return mmc3.reg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(revision == Revision::TR1ROM || revision == Revision::TVROM) {
      if(addr & 0x2000) return read(chrram, addr);
      return read(chrrom, mmc3.chr_addr(addr));
    }
    if(addr & 0x2000) return ppu.ciramRead(ciram_addr(addr));
    if(revision == Revision::TQROM) {
      if(mmc3.chr_addr(addr) & (0x40 << 10))
        return read(chrram, mmc3.chr_addr(addr));
      else
        return read(chrrom, mmc3.chr_addr(addr));
    }
    return Board::chr_read(mmc3.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(revision == Revision::TR1ROM || revision == Revision::TVROM) {
      if(addr & 0x2000) write(chrram, addr, data);
      return;
    }
    if(addr & 0x2000) return ppu.ciramWrite(ciram_addr(addr), data);
    return Board::chr_write(mmc3.chr_addr(addr), data);
  }

  auto ciram_addr(uint addr) -> uint {
    switch(revision) {
    default:
      return mmc3.ciram_addr(addr);
    case Revision::TKSROM:
    case Revision::TLSROM:
      return ((mmc3.chr_addr(addr & 0xfff) & 0x20000) >> 7) | (addr & 0x3ff);
    }
  }

  auto power() -> void {
    mmc3.power();
  }

  auto reset() -> void {
    mmc3.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    mmc3.serialize(s);
  }

  enum class Revision : uint {
    TBROM,
    TEROM,
    TFROM,
    TGROM,
    TKROM,
    TKEPROM,
    TKSROM,
    TLROM,
    TL1ROM,
    TL2ROM,
    TLSROM,
    TNROM,
    TQROM,
    TR1ROM,
    TSROM,
    TVROM,
    MCACC,
  } revision;

  MMC3 mmc3;
};
