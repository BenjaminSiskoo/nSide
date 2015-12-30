struct Namco163 : Board {
  Namco163(Markup::Node& board_node) : Board(board_node), n163(*this, board_node) {
    chip = &n163;
    string name = board_node["chip/ram/name"].text();
    uint size = board_node["chip/ram/size"].natural();
    n163.ram.map(allocate<uint8>(size, 0xff), size);
    if(!name.empty()) {
      interface->loadRequest(ID::ChipRAM, name, false);
      cartridge.memory.append({ID::ChipRAM, name});
    }
  }

  auto enter() -> void {
    n163.enter();
  }

  auto prg_read(uint addr) -> uint8 {
    n163.prg_read(addr);
  }

  auto prg_write(uint addr, uint8 data) -> void {
    n163.prg_write(addr, data);
  }

  auto chr_read(uint addr) -> uint8 {
    n163.chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    n163.chr_write(addr, data);
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);
    n163.serialize(s);
  }

  auto power() -> void {
    n163.power();
  }

  auto reset() -> void {
    n163.reset();
  }

  N163 n163;
};
