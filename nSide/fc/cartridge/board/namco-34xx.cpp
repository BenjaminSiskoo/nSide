//NAMCOT-3401 (bankswitching used even if PRG is 32KB - バベルの塔/Tower of Babel)
//NAMCOT-3406
//NAMCOT-3407
//NAMCOT-3413
//NAMCOT-3414
//NAMCOT-3415
//NAMCOT-3416
//NAMCOT-3417
//NAMCOT-3425
//NAMCOT-3443
//NAMCOT-3446
//NAMCOT-3451
//NAMCOT-3453
//NES-DEROM
//NES-DE1ROM
//NES-DRROM

struct Namco34xx : Board {
  Namco34xx(Markup::Node& board_node) : Board(board_node), n108(*this, board_node) {
    string type = board_node["id"].text();
    if(type.match("*3401")) revision = Revision::Namco3401;
    if(type.match("*3406")) revision = Revision::Namco3406;
    if(type.match("*3407")) revision = Revision::Namco3407;
    if(type.match("*3413")) revision = Revision::Namco3413;
    if(type.match("*3414")) revision = Revision::Namco3414;
    if(type.match("*3415")) revision = Revision::Namco3415;
    if(type.match("*3416")) revision = Revision::Namco3416;
    if(type.match("*3417")) revision = Revision::Namco3417;
    if(type.match("*3425")) revision = Revision::Namco3425;
    if(type.match("*3443")) revision = Revision::Namco3443;
    if(type.match("*3446")) revision = Revision::Namco3446;
    if(type.match("*3451")) revision = Revision::Namco3451;
    if(type.match("*3453")) revision = Revision::Namco3453;
    if(type.match("*DEROM")) revision = Revision::DEROM;
    if(type.match("*DE1ROM")) revision = Revision::DE1ROM;
    if(type.match("*DRROM")) revision = Revision::DRROM;
    if(type == "TENGEN-800002") revision = Revision::DEROM;
    if(type == "TENGEN-800030") revision = Revision::DE1ROM;
    if(type == "TENGEN-800004") revision = Revision::DRROM;

    if(revision != Revision::Namco3425 && revision != Revision::Namco3453)
      settings.mirror = board_node["mirror/mode"].text() == "horizontal";
  }

  auto prg_addr(uint addr) -> uint {
    if(revision == Revision::Namco3401 || prgrom.size() > 0x8000) return n108.prg_addr(addr);
    else return addr & 0x7fff;
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000) return read(prgrom, prg_addr(addr));
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      n108.reg_write(addr, data);
      if(revision == Revision::Namco3453) nametable = data & 0x40;
    }
  }

  auto chr_addr(uint addr) -> uint {
    switch(revision) {
    case Revision::Namco3443:
    case Revision::Namco3453:
      return n108.chr_addr(addr) | ((addr & 0x1000) << 4);
    case Revision::Namco3446:
      if(addr <= 0x07ff) return (n108.chr_addr(0x1000) << 1) | (addr & 0x07ff);
      if(addr <= 0x0fff) return (n108.chr_addr(0x1400) << 1) | (addr & 0x07ff);
      if(addr <= 0x17ff) return (n108.chr_addr(0x1800) << 1) | (addr & 0x07ff);
      if(addr <= 0x1fff) return (n108.chr_addr(0x1c00) << 1) | (addr & 0x07ff);
    default:
      return n108.chr_addr(addr);
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(revision == Revision::DRROM) {
      if(addr & 0x2000) {
        if(!(addr & 0x0800)) return ppu.ciramRead(addr & 0x07ff);
        else                 return read(chrram, addr & 0x07ff);
      }
      return read(chrrom, n108.chr_addr(addr));
    }
    if(addr & 0x2000) return ppu.ciramRead(ciram_addr(addr));
    return Board::chr_read(chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(revision == Revision::DRROM) {
      if(addr & 0x2000) {
        if(!(addr & 0x0800)) ppu.ciramWrite(addr & 0x07ff, data);
        else                 write(chrram, addr & 0x07ff, data);
      }
      return;
    }
    if(addr & 0x2000) return ppu.ciramWrite(ciram_addr(addr), data);
    return Board::chr_write(chr_addr(addr), data);
  }

  auto ciram_addr(uint addr) const -> uint {
    switch(revision) {
    default:
      if(settings.mirror == 0) return ((addr & 0x0400) >> 0) | (addr & 0x03ff);
      if(settings.mirror == 1) return ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    case Revision::Namco3425:
      return ((n108.chr_addr(addr & 0x1fff) & 0x8000) >> 5) | (addr & 0x03ff);
    case Revision::Namco3453:
      return (addr & 0x03ff) | (nametable ? 0x0400 : 0x0000);
    }
  }

  auto reset() -> void {
    nametable = 0;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    if(revision == Revision::Namco3453) s.integer(nametable);
  }

  enum class Revision : uint {
    Namco3401,
    Namco3406,
    Namco3407,
    Namco3413,
    Namco3414,
    Namco3415,
    Namco3416,
    Namco3417,
    Namco3425,
    Namco3443,
    Namco3446,
    Namco3451,
    Namco3453,
    DEROM,
    DE1ROM,
    DRROM,
  } revision;

  struct Settings {
    bool mirror;  //0 = vertical, 1 = horizontal
  } settings;

  N108 n108;

  bool nametable; // for Namco3453, used only by Devil Man
};
