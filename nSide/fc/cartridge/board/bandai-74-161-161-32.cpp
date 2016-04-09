  //BANDAI-74*161/161/32

struct Bandai74_161_161_32 : Board {
  Bandai74_161_161_32(Markup::Node& board_node) : Board(board_node) {
    settings.mirror = board_node["mirror/mode"].text() == "horizontal";
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x0000) return cpu.mdr();
    if((addr & 0xc000) == 0x8000) return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
    else                          return read(prgrom, (    0x0f << 14) | (addr & 0x3fff));
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      //TODO: check for bus conflicts
      data &= prg_read(addr);
      prg_bank = (data & 0xf0) >> 4;
      chr_bank = (data & 0x0f) >> 0;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciramRead(addr);
    }
    addr = (chr_bank * 0x2000) + (addr & 0x1fff);
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciramWrite(addr, data);
    }
    addr = (chr_bank * 0x2000) + (addr & 0x1fff);
    Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
    chr_bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prg_bank);
    s.integer(chr_bank);
  }

  struct Settings {
    bool mirror;  //0 = vertical, 1 = horizontal
  } settings;

  uint4 prg_bank;
  uint4 chr_bank;
};
