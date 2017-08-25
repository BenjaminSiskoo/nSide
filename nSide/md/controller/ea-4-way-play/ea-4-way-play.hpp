struct EA4WayPlay : Controller {
  enum : uint {
  //Up, Down, Left, Right, A, B, C, X, Y, Z, Mode, Start,
    Up, Down, Left, Right, A, B, C, Start,
  };

  EA4WayPlay(uint port);

//auto main() -> void override;
  auto readData() -> uint8 override;
  auto writeData(uint8 data) -> void override;

  static uint2 player;
  static boolean signature;
  static boolean select;
//static boolean localSelect[4];
//static uint3 counter[4];
//static uint timeout[4];
  boolean latch;
};
