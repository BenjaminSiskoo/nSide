struct Gamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Select, Start,
  };

  Gamepad(bool port, uint index);

  auto data() -> uint3;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool a, b, select, start;
  bool up, down, left, right;
};
