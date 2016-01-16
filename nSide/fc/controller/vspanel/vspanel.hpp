struct VSPanel : Controller {
  enum : uint {
    Button1, Button2, Button3, Button4, ServiceButton, Coin1, Coin2,
  };

  VSPanel(uint port);

  auto data() -> uint5;
  auto data1() -> uint2;
  auto data2() -> uint5;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter1;
  uint counter2;

  bool b1, b2, b3, b4;
};
