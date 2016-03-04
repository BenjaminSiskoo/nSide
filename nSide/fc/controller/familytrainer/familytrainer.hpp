struct FamilyTrainer : Controller {
  FamilyTrainer(uint port);

  auto data() -> uint5;
  auto data1() -> uint2;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool b1, b2, b3, b4;
  bool b5, b6, b7, b8;
  bool b9, b10, b11, b12;
};