//JALECO-JF-24
//JALECO-JF-25

struct JalecoJF2x : Board {
  JalecoJF2x(Markup::Node& board_node) : Board(board_node), ss88006(*this, board_node) {
    string type = board_node["id"].text();
    if(type.match("*JF-23")) revision = Revision::JF_23;
    if(type.match("*JF-24")) revision = Revision::JF_24;
    if(type.match("*JF-25")) revision = Revision::JF_25;
    if(type.match("*JF-27")) revision = Revision::JF_27;
    if(type.match("*JF-29")) revision = Revision::JF_29;
    if(type.match("*JF-37")) revision = Revision::JF_37;
    if(type.match("*JF-40")) revision = Revision::JF_40;
  }

  auto enter() -> void {
    ss88006.enter();
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0x8000) == 0x8000) return read(prgrom, ss88006.prg_addr(addr));
    if((addr & 0xe000) == 0x6000) return ss88006.ram_read(addr);
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0x8000) == 0x8000) return ss88006.reg_write(addr, data);
    if((addr & 0xe000) == 0x6000) return ss88006.ram_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciram_read(ss88006.ciram_addr(addr));
    return Board::chr_read(ss88006.chr_addr(addr));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciram_write(ss88006.ciram_addr(addr), data);
    return Board::chr_write(ss88006.chr_addr(addr), data);
  }

  auto power() -> void {
    ss88006.power();
  }

  auto reset() -> void {
    ss88006.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    ss88006.serialize(s);
  }

  enum class Revision : uint {
    JF_23,
    JF_24,
    JF_25,
    JF_27,
    JF_29,
    JF_37,
    JF_40,
  } revision;

  SS88006 ss88006;
};
