struct FightingPad6B : Controller {
  enum : uint {
    Up, Down, Left, Right, A, B, C, X, Y, Z, Start, Mode,
  };

  FightingPad6B(bool port);

  auto main() -> void;
  auto read() -> uint7;
  auto write(uint7 data) -> void;

private:
  bool th;
  uint counter;
  uint timeout;
};
