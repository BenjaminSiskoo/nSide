struct FightingPad6B : Controller {
  enum : uint {
    Up, Down, Left, Right, A, B, C, X, Y, Z, Start, Mode,
  };

  FightingPad6B(uint port);

  auto main() -> void override;
  auto readData() -> uint8 override;
  auto writeData(uint8 data) -> void override;

  boolean select;
  boolean latch;
  uint3 counter;
  uint timeout;
};
