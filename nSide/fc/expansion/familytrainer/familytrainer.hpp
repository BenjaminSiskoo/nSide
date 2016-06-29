struct FamilyTrainer : Expansion {
  FamilyTrainer();

  auto data1() -> bool;
  auto data2() -> uint5;
  auto write(uint3 data) -> void;

private:
  bool latched;
  uint counter;

  boolean b1, b2,  b3,  b4;
  boolean b5, b6,  b7,  b8;
  boolean b9, b10, b11, b12;
};
