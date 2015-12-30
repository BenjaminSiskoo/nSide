struct HVC_ExROM : Board {
  HVC_ExROM(Markup::Node& board_node) : Board(board_node), mmc5(*this, board_node) {
    chip = &mmc5;
    if(board_node["chip/ram"]) {
      string name = board_node["chip/ram/name"].text();
      uint size = board_node["chip/ram/size"].natural();
      mmc5.ram.map(allocate<uint8>(size, 0xff), size);
      if(!name.empty()) {
        interface->loadRequest(ID::ChipRAM, name, false);
        cartridge.memory.append({ID::ChipRAM, name});
      }
    } else {
      mmc5.ram.map(allocate<uint8>(1024, 0xff), 1024);
    }
    string type = board_node["id"].text();
    if(type.match("*EKROM*")) revision = Revision::EKROM;
    if(type.match("*ELROM*")) revision = Revision::ELROM;
    if(type.match("*ETROM*")) revision = Revision::ETROM;
    if(type.match("*EWROM*")) revision = Revision::EWROM;
  }

  auto enter() -> void {
    mmc5.enter();
  }

  auto prg_read(uint addr) -> uint8 {
    return mmc5.prg_read(addr);
  }

  auto prg_write(uint addr, uint8 data) -> void {
    mmc5.prg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    return mmc5.chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    mmc5.chr_write(addr, data);
  }

  auto scanline(uint y) -> void {
    mmc5.scanline(y);
  }

  auto power() -> void {
    mmc5.power();
  }

  auto reset() -> void {
    mmc5.reset();
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    mmc5.serialize(s);
  }

  enum class Revision : uint {
    EKROM,
    ELROM,
    ETROM,
    EWROM,
  } revision;

  MMC5 mmc5;
};
