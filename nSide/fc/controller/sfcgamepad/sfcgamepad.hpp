struct SFCGamepad : Controller {
  enum : uint {
    B, Y, Select, Start, Up, Down, Left, Right, A, X, L, R,
  };

  SFCGamepad(uint port);

  auto data() -> uint5;
  auto data1() -> uint2;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool b, y, select, start;
  bool up, down, left, right;
  bool a, x, l, r;
};
