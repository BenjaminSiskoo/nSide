//HVC-CNROM
//NES-CPROM

struct HVC_CxROM : Board {
  HVC_CxROM(Markup::Node& board_node) : Board(board_node) {
    string type = board_node["id"].text();
    if(type.match("*CNROM" )) revision = Revision::CNROM;
    if(type.match("*CPROM" )) revision = Revision::CPROM;
    if(type.match("*PT-554")) revision = Revision::CNROM;
    if(type == "TENGEN-800008") revision = Revision::CNROM;

    settings.mirror = board_node["mirror/mode"].text() == "horizontal";
    settings.security = (bool)board_node["security"];
    if(settings.security) {
      settings.pass = board_node["security/pass"].natural();
    }
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, addr & 0x7fff);
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      data &= prg_read(addr); // Bus conflicts
      chr_bank = data & 0x03;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr);
    }
    if(settings.security) {
      if(chr_bank != settings.pass) return ppu.status.mdr;
    }
    switch(revision) {
    case Revision::CNROM:
      addr = (chr_bank * 0x2000) + (addr & 0x1fff);
      break;
    case Revision::CPROM:
      if(addr < 0x1000) addr = addr & 0x1fff;
      else              addr = (chr_bank * 0x1000) + (addr & 0x0fff);
      break;
    }
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(settings.mirror == 1) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr, data);
    }
    switch(revision) {
    case Revision::CNROM:
      addr = (chr_bank * 0x2000) + (addr & 0x1fff);
      break;
    case Revision::CPROM:
      if(addr < 0x1000) addr = addr & 0x1fff;
      else              addr = (chr_bank * 0x1000) + (addr & 0x0fff);
      break;
    }
    Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    chr_bank = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    s.integer(chr_bank);
  }

  enum class Revision : uint {
    CNROM,
    CPROM,
  } revision;

  struct Settings {
    bool mirror;  //0 = vertical, 1 = horizontal
    bool security;
    uint2 pass;
  } settings;

  uint2 chr_bank;
};
