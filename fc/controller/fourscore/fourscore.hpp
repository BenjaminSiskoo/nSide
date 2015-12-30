struct FourScore : Controller {
  enum : uint {
    A, B, Select, Start, Up, Down, Left, Right,
  };

  FourScore(uint port);

  auto data() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool aL, bL, selectL, startL;
  bool upL, downL, leftL, rightL;
  bool aR, bR, selectR, startR;
  bool upR, downR, leftR, rightR;
};
