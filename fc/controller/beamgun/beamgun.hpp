struct BeamGun : Controller {
  void enter();
  uint5 data();
  uint2 data1();
  uint5 data2();
  bool read_light();
  void latch(bool data);
  BeamGun(unsigned port);

//private:
  bool latched;
  unsigned counter; // VS. System

  signed x, y;
  bool light;
  bool trigger;
  unsigned lighttime;
  unsigned triggertime;

  bool offscreen;

  bool triggerlock;
};
