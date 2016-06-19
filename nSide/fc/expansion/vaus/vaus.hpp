struct VausE : Expansion {
  enum : uint {
    Control, Fire,
  };

  VausE(uint index);

  auto main() -> void;
  auto data1() -> bool;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  int x;
  uint8 control;

  uint prev;
};
