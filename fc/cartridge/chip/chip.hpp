struct Board;

struct Chip {
  Board& board;
  MappedRAM ram;
  void tick();
  Chip(Board& board);
};
