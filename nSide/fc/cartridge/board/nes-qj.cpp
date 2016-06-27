struct NES_QJ : Board {
  NES_QJ(Markup::Node& boardNode) : Board(boardNode), mmc3(*this, boardNode) {
  }

  auto main() -> void {
    mmc3.main();
  }

  auto readPRG(uint addr) -> uint8 {
    if(addr & 0x8000) {
      addr = (mmc3.prgAddress(addr) & 0x1ffff) | (bank << 17);
      return read(prgrom, addr);
    }
    return cpu.mdr();
  }

  auto writePRG(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000 && mmc3.ramEnable && !mmc3.ramWriteProtect) {
      bank = data & 0x01;
    }
    if(addr & 0x8000) return mmc3.regWrite(addr, data);
  }

  auto readCHR(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.readCIRAM(mmc3.ciramAddress(addr));
    return Board::readCHR((mmc3.chrAddress(addr) & 0x1ffff) | (bank << 17));
  }

  auto writeCHR(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.writeCIRAM(mmc3.ciramAddress(addr), data);
    return Board::writeCHR((mmc3.chrAddress(addr) & 0x1ffff) | (bank << 17), data);
  }

  auto ciramAddress(uint addr) -> uint {
    return mmc3.ciramAddress(addr);
  }

  auto power() -> void {
    mmc3.power();
  }

  auto reset() -> void {
    mmc3.reset();
    bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    mmc3.serialize(s);
    s.integer(bank);
  }

  MMC3 mmc3;
  bool bank;
};
