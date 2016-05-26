struct FourPlayers : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Select, Start,
  };

  FourPlayers(uint port);

  auto data1() -> bool;
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
