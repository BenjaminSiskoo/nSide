struct Board;

struct Chip {
  Board& board;
  MappedRAM ram;
  auto tick() -> void;
  Chip(Board& board);
};
