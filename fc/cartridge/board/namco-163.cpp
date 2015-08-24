struct Namco163 : Board {

N163 n163;

void enter() {
  n163.enter();
}

uint8 prg_read(unsigned addr) {
  n163.prg_read(addr);
}

void prg_write(unsigned addr, uint8 data) {
  n163.prg_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  n163.chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  n163.chr_write(addr, data);
}

void serialize(serializer& s) {
  Board::serialize(s);
  n163.serialize(s);
}

void power() {
  n163.power();
}

void reset() {
  n163.reset();
}

Namco163(Markup::Node& cartridge) : Board(cartridge), n163(*this, cartridge) {
  chip = &n163;
  string name = cartridge["chip/ram/name"].text();
  unsigned size = cartridge["chip/ram/size"].decimal();
  n163.ram.map(allocate<uint8>(size, 0xff), size);
  if(!name.empty()) {
    interface->loadRequest(ID::ChipRAM, name, false);
    Famicom::cartridge.memory.append({ID::ChipRAM, name});
  }
}

};
