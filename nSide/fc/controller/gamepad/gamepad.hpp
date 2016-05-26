struct Gamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Select, Start,
  };

  Gamepad(uint port);

  auto data() -> uint5;
  auto data1() -> bool;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool a, b, select, start;
  bool up, down, left, right;
};
