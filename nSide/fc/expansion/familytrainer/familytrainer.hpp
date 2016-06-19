struct FamilyTrainer : Expansion {
  FamilyTrainer(uint index);

  auto data1() -> bool;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool b1, b2,  b3,  b4;
  bool b5, b6,  b7,  b8;
  bool b9, b10, b11, b12;
};
