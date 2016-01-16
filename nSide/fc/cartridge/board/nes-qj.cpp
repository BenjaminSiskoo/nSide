struct NES_QJ : Board {
  NES_QJ(Markup::Node& board_node) : Board(board_node), mmc3(*this, board_node) {
  }

  auto enter() -> void {
    mmc3.enter();
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) {
      addr = (mmc3.prg_addr(addr) & 0x1ffff) | (bank << 17);
      return read(prgrom, addr);
    }
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000 && mmc3.ram_enable && !mmc3.ram_write_protect) {
      bank = data & 0x01;
    }
    if(addr & 0x8000) return mmc3.reg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciram_read(mmc3.ciram_addr(addr));
    return Board::chr_read((mmc3.chr_addr(addr) & 0x1ffff) | (bank << 17));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciram_write(mmc3.ciram_addr(addr), data);
    return Board::chr_write((mmc3.chr_addr(addr) & 0x1ffff) | (bank << 17), data);
  }

  auto ciram_addr(uint addr) -> uint {
    return mmc3.ciram_addr(addr);
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
