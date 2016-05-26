struct FourScore : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Select, Start,
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
