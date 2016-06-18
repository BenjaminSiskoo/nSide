struct PAL_ZZ : Board {
  PAL_ZZ(Markup::Node& boardNode) : Board(boardNode), mmc3(*this, boardNode) {
  }

  auto main() -> void {
    mmc3.main();
  }

  auto prgRead(uint addr) -> uint8 {
    if(addr & 0x8000) {
      addr = (mmc3.prgAddress(addr) & (0xffff | (bank << 16))) | (bank << 17);
      addr |= (prgA16Force == 0x03) << 16;
      return read(prgrom, addr);
    }
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000 && mmc3.ramEnable && !mmc3.ramWriteProtect) {
      prgA16Force = data & 0x03;
      bank = data & 0x04;
    }
    if(addr & 0x8000) return mmc3.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(mmc3.ciramAddress(addr));
    return Board::chrRead((mmc3.chrAddress(addr) & 0x1ffff) | (bank << 17));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(mmc3.ciramAddress(addr), data);
    return Board::chrWrite((mmc3.chrAddress(addr) & 0x1ffff) | (bank << 17), data);
  }

  auto ciramAddress(uint addr) -> uint {
    return mmc3.ciramAddress(addr);
  }

  auto power() -> void {
    mmc3.power();
  }

  auto reset() -> void {
    mmc3.reset();
    cicReset();
  }

  auto cicReset() -> void {
    // this register is cleared by the CIC reset line.
    // On a Famicom or toploader, only a power cycle can clear it.
    //TODO: Check if Europe got the toploader.
    prgA16Force = 0;
    bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    mmc3.serialize(s);
    s.integer(prgA16Force);
    s.integer(bank);
  }

  MMC3 mmc3;
  uint2 prgA16Force;
  bool bank;
};
