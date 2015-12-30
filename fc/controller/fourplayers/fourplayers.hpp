struct FourPlayers : Controller {
  enum : uint {
    A, B, Select, Start, Up, Down, Left, Right,
  };

  FourPlayers(uint port);

  auto data1() -> uint2;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter1;
  uint counter2;

  bool a3, b3, select3, start3;
  bool up3, down3, left3, right3;
  bool a4, b4, select4, start4;
  bool up4, down4, left4, right4;
};
