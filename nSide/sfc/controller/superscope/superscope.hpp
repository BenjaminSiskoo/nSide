struct SuperScope : Controller {
  enum : uint {
    X, Y, Trigger, Cursor, Turbo, Pause,
  };

  SuperScope(bool port);

  auto main() -> void;
  auto data() -> uint2;
  auto latch(bool data) -> void;

//private:
  bool latched;
  unsigned counter;

  signed x, y;

  bool trigger;
  bool cursor;
  bool turbo;
  bool pause;
  bool offscreen;

  bool turbolock;
  bool triggerlock;
  bool pauselock;

  unsigned prev;

  Emulator::Video::Cursor videoCursor;
  static const uint64 normalPalette[3];
  static const uint64 turboPalette[6];
  static const uint8 normalData[15 * 15];
  static const uint8 turboData[15 * 15];
};
