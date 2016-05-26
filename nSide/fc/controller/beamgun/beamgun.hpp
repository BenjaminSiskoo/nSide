struct BeamGun : Controller {
  enum : uint {
    X, Y, Trigger,
  };

  BeamGun(uint port);

  auto main() -> void;
  auto data() -> uint5;
  auto data1() -> bool;
  auto data2() -> uint5;
  auto readLight() -> bool;
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

  uint prev;

  Emulator::Video::Cursor cursor;
  static const uint64 beamgunPalette[3];
  static const uint64 zapperPalette[3];
  static const uint8 cursorData[15 * 15];
};
