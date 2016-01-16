struct Gamepad : Controller {
  enum : uint {
    A, B, Select, Start, Up, Down, Left, Right,
  };

  Gamepad(uint port);

  auto data() -> uint5;
  auto data1() -> uint2;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool a, b, select, start;
  bool up, down, left, right;
};
