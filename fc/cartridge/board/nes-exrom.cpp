struct NES_ExROM : Board {

enum class Revision : unsigned {
  EKROM,
  ELROM,
  ETROM,
  EWROM,
} revision;

MMC5 mmc5;

void enter() {
  mmc5.enter();
}

uint8 prg_read(unsigned addr) {
  return mmc5.prg_read(addr);
}

void prg_write(unsigned addr, uint8 data) {
  mmc5.prg_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  return mmc5.chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  mmc5.chr_write(addr, data);
}

void scanline(unsigned y) {
  mmc5.scanline(y);
}

void power() {
  mmc5.power();
}

void reset() {
  mmc5.reset();
}

void serialize(serializer& s) {
  Board::serialize(s);
  mmc5.serialize(s);
}

NES_ExROM(Markup::Node& cartridge) : Board(cartridge), mmc5(*this, cartridge) {
  chip = &mmc5;
  if(cartridge["chip/ram"]) {
    string name = cartridge["chip/ram/name"].text();
    unsigned size = cartridge["chip/ram/size"].decimal();
    mmc5.ram.map(allocate<uint8>(size, 0xff), size);
    if(!name.empty()) {
      interface->loadRequest(ID::ChipRAM, name, false);
      Famicom::cartridge.memory.append({ID::ChipRAM, name});
    }
  } else {
    mmc5.ram.map(allocate<uint8>(1024, 0xff), 1024);
  }
  string type = cartridge["board/type"].text();
  if(type.match("*EKROM*")) revision = Revision::EKROM;
  if(type.match("*ELROM*")) revision = Revision::ELROM;
  if(type.match("*ETROM*")) revision = Revision::ETROM;
  if(type.match("*EWROM*")) revision = Revision::EWROM;
}

};
