struct Namco163 : Board {
  Namco163(Markup::Node& boardNode) : Board(boardNode), n163(*this, boardNode) {
    chip = &n163;
  }

  auto main() -> void {
    n163.main();
  }

  auto prgRead(uint addr) -> uint8 {
    n163.prgRead(addr);
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    n163.prgWrite(addr, data);
  }

  auto chrRead(uint addr) -> uint8 {
    n163.chrRead(addr);
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    n163.chrWrite(addr, data);
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
