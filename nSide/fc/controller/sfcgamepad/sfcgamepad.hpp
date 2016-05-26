struct SFCGamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  SFCGamepad(uint port);

  auto data() -> uint5;
  auto data1() -> bool;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool b, y, select, start;
  bool up, down, left, right;
  bool a, x, l, r;
};
