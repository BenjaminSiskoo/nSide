//BANDAI-FCG
//BANDAI-FCG-1
//BANDAI-FCG-2
//BANDAI-JUMP2
//BANDAI-LZ93D50

struct BandaiFCG : Board {
  BandaiFCG(Markup::Node& boardNode) : Board(boardNode), fcg(*this, boardNode) {
    string type = boardNode["id"].text();
    revision = Revision::FCGAll;
    if(type.match("*FCG-1*"  )) revision = Revision::FCG1;
    if(type.match("*FCG-2*"  )) revision = Revision::FCG2;
    if(type.match("*JUMP2*"  )) revision = Revision::JUMP2;
    if(type.match("*LZ93D50*")) revision = Revision::LZ93D50;
  }

  auto main() -> void {
    fcg.main();
  }

  auto prgRead(uint addr) -> uint8 {
    if((addr & 0xe000) == 0x6000) {
      switch(revision) {
      case Revision::LZ93D50:
        //TODO: serial EEPROM support
        return 0x00 | (cpu.mdr() & 0xef);
      case Revision::JUMP2:
        return fcg.eepromI2C_SCL ? fcg.ramRead(addr) : cpu.mdr();
      }
    }
    if((addr & 0x8000) == 0x8000) {
      if(revision != Revision::JUMP2)
        return read(prgrom, fcg.prgAddress(addr));
      else
        return read(prgrom, fcg.prgAddress(addr) | ((fcg.chrBank[(ppu.status.chrAddressBus >> 10) & 3] & 1) << 18));
    }
    return cpu.mdr();
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) {
      switch(revision) {
      case Revision::FCGAll:
      case Revision::FCG1:
      case Revision::FCG2:
        return fcg.regWrite(addr, data);
      case Revision::LZ93D50:
        //TODO: serial EEPROM support
        break;
      case Revision::JUMP2:
        if(fcg.eepromI2C_SCL) return fcg.ramWrite(addr, data);
        else                   break;
      }
    }
    if((addr & 0x8000) == 0x8000) {
      switch(revision) {
      case Revision::FCGAll:
      case Revision::LZ93D50:
      case Revision::JUMP2:
        return fcg.regWrite(addr, data);
      }
    }
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) return ppu.ciramRead(fcg.ciramAddress(addr));
    if(chrrom.size()) return Board::chrRead(fcg.chrAddress(addr));
    if(chrram.size()) return Board::chrRead(addr);
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) return ppu.ciramWrite(fcg.ciramAddress(addr), data);
    if(chrram.size()) Board::chrWrite(addr, data);
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
