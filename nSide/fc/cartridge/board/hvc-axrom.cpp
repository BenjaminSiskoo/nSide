//HVC-AMROM
//NES-ANROM
//NES-AN1ROM
//HVC-AOROM

struct HVC_AxROM : Board {
  HVC_AxROM(Markup::Node& board_node) : Board(board_node) {
    string type = board_node["id"].text();
    if(type.match("*AMROM" )) revision = Revision::AMROM;
    if(type.match("*ANROM" )) revision = Revision::ANROM;
    if(type.match("*AN1ROM")) revision = Revision::AN1ROM;
    if(type.match("*AOROM" )) revision = Revision::AOROM;
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      // Bus conflicts
      if(revision == Revision::AMROM) data &= prg_read(addr);
      prg_bank = data & 0x0f;
      mirror_select = data & 0x10;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciram_read((mirror_select << 10) | (addr & 0x03ff));
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciram_write((mirror_select << 10) | (addr & 0x03ff), data);
    return Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank = 0x0f;
    mirror_select = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);

    s.integer(prg_bank);
    s.integer(mirror_select);
  }

  enum class Revision : uint {
    AMROM,
    ANROM,
    AN1ROM,
    AOROM,
  } revision;

  uint4 prg_bank;
  bool mirror_select;
};
