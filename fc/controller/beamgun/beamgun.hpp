struct BeamGun : Controller {
  enum : uint {
    X, Y, Trigger,
  };

  BeamGun(uint port);

  auto enter() -> void;
  auto data() -> uint5;
  auto data1() -> uint2;
  auto data2() -> uint5;
  auto read_light() -> bool;
  auto latch(bool data) -> void;

//private:
  bool latched;
  uint counter; // VS. System

  int x, y;
  bool light;
  bool trigger;
  uint lighttime;
  uint triggertime;
  bool offscreen;

  bool triggerlock;
};
