struct SegaTap : Controller {
  enum : uint {
    Up, Down, Left, Right, A, B, C, X, Y, Z, Start, Mode,
  };

  SegaTap(uint port);

  auto readData() -> uint8 override;
  auto writeData(uint8 data) -> void override;

  uint4 buffer[24];

  boolean tr;
  boolean th;
  boolean latch;
  uint5 counter;
};
