struct NES_HKROM : Board {
  NES_HKROM(Markup::Node& boardNode) : Board(boardNode), mmc6(*this) {
  }

  auto main() -> void {
    mmc6.main();
  }

  auto prgRead(uint addr) -> uint8 {
    if((addr & 0xf000) == 0x7000) return mmc6.ramRead(addr);
    if(addr & 0x8000) return read(prgrom, mmc6.prgAddress(addr));
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if((addr & 0xf000) == 0x7000) return mmc6.ramWrite(addr, data);
    if(addr & 0x8000) return mmc6.regWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    mmc6.irqTest(addr);
    if(addr & 0x2000) return ppu.ciramRead(mmc6.ciramAddress(addr));
    return Board::chrRead(mmc6.chrAddress(addr));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    mmc6.irqTest(addr);
    if(addr & 0x2000) return ppu.ciramWrite(mmc6.ciramAddress(addr), data);
    return Board::chrWrite(mmc6.chrAddress(addr), data);
  }

  auto power() -> void {
    mmc6.power();
  }

  auto reset() -> void {
    mmc6.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    mmc6.serialize(s);
  }

  MMC6 mmc6;
};
