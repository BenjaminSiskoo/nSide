//BANDAI-FCG
//BANDAI-FCG-1
//BANDAI-FCG-2
//BANDAI-JUMP2
//BANDAI-LZ93D50

struct BandaiFCG : Board {
  BandaiFCG(Markup::Node& board_node) : Board(board_node), fcg(*this, board_node) {
    string type = board_node["id"].text();
    revision = Revision::FCGAll;
    if(type.match("*FCG-1*"  )) revision = Revision::FCG1;
    if(type.match("*FCG-2*"  )) revision = Revision::FCG2;
    if(type.match("*JUMP2*"  )) revision = Revision::JUMP2;
    if(type.match("*LZ93D50*")) revision = Revision::LZ93D50;
  }

  auto main() -> void {
    fcg.main();
  }

  auto prg_read(uint addr) -> uint8 {
    if((addr & 0xe000) == 0x6000) {
      switch(revision) {
      case Revision::LZ93D50:
        //TODO: serial EEPROM support
        return 0x00 | (cpu.mdr() & 0xef);
      case Revision::JUMP2:
        return fcg.eeprom_i2c_scl ? fcg.ram_read(addr) : cpu.mdr();
      }
    }
    if((addr & 0x8000) == 0x8000) {
      if(revision != Revision::JUMP2)
        return read(prgrom, fcg.prg_addr(addr));
      else
        return read(prgrom, fcg.prg_addr(addr) | ((fcg.chr_bank[(ppu.status.chr_abus >> 10) & 3] & 1) << 18));
    }
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) {
      switch(revision) {
      case Revision::FCGAll:
      case Revision::FCG1:
      case Revision::FCG2:
        return fcg.reg_write(addr, data);
      case Revision::LZ93D50:
        //TODO: serial EEPROM support
        break;
      case Revision::JUMP2:
        if(fcg.eeprom_i2c_scl) return fcg.ram_write(addr, data);
        else                   break;
      }
    }
    if((addr & 0x8000) == 0x8000) {
      switch(revision) {
      case Revision::FCGAll:
      case Revision::LZ93D50:
      case Revision::JUMP2:
        return fcg.reg_write(addr, data);
      }
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciram_read(fcg.ciram_addr(addr));
    if(chrrom.size()) return Board::chr_read(fcg.chr_addr(addr));
    if(chrram.size()) return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciram_write(fcg.ciram_addr(addr), data);
    if(chrram.size()) Board::chr_write(addr, data);
  }

  auto power() -> void {
    fcg.power();
  }

  auto reset() -> void {
    fcg.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    fcg.serialize(s);
  }

  enum class Revision : uint {
    FCGAll,
    FCG1,
    FCG2,
    JUMP2,
    LZ93D50,
  } revision;

  FCG fcg;
};
