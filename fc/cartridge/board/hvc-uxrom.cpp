//HVC-UNROM
//HVC-UN1ROM
//HVC-UOROM

struct HVC_UxROM : Board {
  HVC_UxROM(Markup::Node& board_node) : Board(board_node) {
    settings.mirror = board_node["mirror/mode"].text() == "horizontal";
    string type = board_node["id"].text();
    if(type.match("*UNROM" )) revision = Revision::UNROM;
    if(type.match("*UN1ROM")) revision = Revision::UN1ROM;
    if(type.match("*UOROM" )) revision = Revision::UOROM;
    type = board_node["chip/type"].text();
    if(type.match("74*32")) chip_type = ChipType::_7432;
    if(type.match("74*08")) chip_type = ChipType::_7408;
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x0000) return cpu.mdr();
    switch(chip_type) {
    case ChipType::_7432:
      switch(addr & 0xc000) {
      case 0x8000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
      case 0xc000: return read(prgrom, (    0x0f << 14) | (addr & 0x3fff));
      }
    case ChipType::_7408:
      switch(addr & 0xc000) {
      case 0x8000: return read(prgrom, (    0x00 << 14) | (addr & 0x3fff));
      case 0xc000: return read(prgrom, (prg_bank << 14) | (addr & 0x3fff));
      }
    }
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      // Bus conflicts
      data &= prg_read(addr);
      if(revision != Revision::UN1ROM) prg_bank = data & 0x0f;
      if(revision == Revision::UN1ROM) prg_bank = (data & 0x1c) >> 2;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr);
    }
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr, data);
    }
    return Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);

    s.integer(prg_bank);
  }

  enum class Revision : uint {
    UNROM,
    UN1ROM,
    UOROM,
  } revision;

  enum class ChipType : uint {
    _7432,
    _7408,
  } chip_type;

  struct Settings {
    bool mirror;    //0 = vertical, 1 = horizontal
  } settings;

  uint4 prg_bank;
};
