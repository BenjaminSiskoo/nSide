struct SNESGamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  SNESGamepad(bool port);

  auto data() -> uint3;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool b, y, select, start;
  bool up, down, left, right;
  bool a, x, l, r;
};
