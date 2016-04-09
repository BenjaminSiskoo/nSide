//JALECO-JF-05
//JALECO-JF-06
//JALECO-JF-07
//JALECO-JF-08
//JALECO-JF-09
//JALECO-JF-10
//JALECO-JF-11
//JALECO-JF-14

struct JalecoJF0x : Board {
  JalecoJF0x(Markup::Node& board_node) : Board(board_node) {
    string type = board_node["id"].text();
    if(type.match("*JF-05*")) revision = Revision::JF_05;
    if(type.match("*JF-06*")) revision = Revision::JF_06;
    if(type.match("*JF-07*")) revision = Revision::JF_07;
    if(type.match("*JF-08*")) revision = Revision::JF_08;
    if(type.match("*JF-09*")) revision = Revision::JF_09;
    if(type.match("*JF-10*")) revision = Revision::JF_10;
    if(type.match("*JF-11*")) revision = Revision::JF_11;
    if(type.match("*JF-14*")) revision = Revision::JF_14;

    settings.mirror = board_node["mirror/mode"].text() == "horizontal";
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) {
      switch(revision) {
      case Revision::JF_05:
      case Revision::JF_06:
      case Revision::JF_07:
      case Revision::JF_08:
      case Revision::JF_09:
      case Revision::JF_10:
        chr_bank = ((data & 0x01) << 1) | ((data & 0x02) >> 1);
        break;
      case Revision::JF_11:
      case Revision::JF_14:
        prg_bank = (data & 0x30) >> 4;
        chr_bank = (data & 0x0f) >> 0;
        break;
      }
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

  enum class Revision : uint {
    JF_05,
    JF_06,
    JF_07,
    JF_08,
    JF_09,
    JF_10,
    JF_11,
    JF_14,
  } revision;

  struct Settings {
    bool mirror;  //0 = vertical, 1 = horizontal
  } settings;

  uint2 prg_bank;
  uint4 chr_bank;
};
