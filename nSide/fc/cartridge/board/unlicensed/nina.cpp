//AVE-NINA-01
//AVE-NINA-02
//AVE-NINA-03
//AVE-NINA-06

struct Nina : Board {
  Nina(Markup::Node& board_node) : Board(board_node) {
    string type = board_node["id"].text();
    if(type.match("*NINA-01")) revision =  1;
    if(type.match("*NINA-02")) revision =  2;
    if(type.match("*NINA-03")) revision =  3;
    if(type.match("*NINA-06")) revision =  6;

    if(revision ==  1 || revision ==  2) {
      settings.mirror = 1;
    } else {
      settings.mirror = board_node["mirror/mode"].text() == "horizontal";
    }
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
    if((addr & 0xe000) == 0x6000) {
      if(prgram.size() > 0) return read(prgram, addr);
    }
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    switch(revision) {
    case  1:
    case  2:
      switch(addr) {
      case 0x7ffd: prg_bank    = data & 0x01; break;
      case 0x7ffe: chr_bank[0] = data & 0x0f; break;
      case 0x7fff: chr_bank[1] = data & 0x0f; break;
      }
      break;
    case  3:
    case  6:
      if((addr & 0xe100) == 0x4100) {
        prg_bank    = (data & 0x08) >> 3;
        chr_bank[0] = ((data & 0x07) << 1) | 0;
        chr_bank[1] = ((data & 0x07) << 1) | 1;
      }
      break;
    }
    if((addr & 0xe000) == 0x6000 && prgram.size() > 0) write(prgram, addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr);
    }
    return Board::chr_read((addr & 0x0fff) | (chr_bank[(addr & 0x1000) >> 12] << 12));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr, data);
    }
    return Board::chr_write((addr & 0x0fff) | (chr_bank[(addr & 0x1000) >> 12] << 12), data);
  }

  auto power() -> void {
    reset();
  }

  auto reset() -> void {
    prg_bank = 0;
    chr_bank[0] = 0;
    chr_bank[1] = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(prg_bank);
    s.array(chr_bank);
  }

  uint revision;

  struct Settings {
    bool mirror;  //0 = vertical, 1 = horizontal
  } settings;

  bool prg_bank;
  uint4 chr_bank[2];
};
