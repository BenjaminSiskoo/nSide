struct GamepadE : Expansion {
  enum : uint {
    Up, Down, Left, Right, B, A, Select, Start,
  };

  GamepadE(uint index);

  auto data1() -> bool;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool a, b, select, start;
  bool up, down, left, right;
};
