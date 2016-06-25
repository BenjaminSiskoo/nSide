struct FourScore : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Select, Start,
  };

  FourScore(bool port);

  auto data() -> uint3;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  bool aL, bL, selectL, startL;
  bool upL, downL, leftL, rightL;
  bool aR, bR, selectR, startR;
  bool upR, downR, leftR, rightR;
};
