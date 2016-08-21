struct Gamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, I, II,
  };

  Gamepad(bool port);

  auto readData() -> uint7 override;
  auto writeData(uint7 data) -> void override;

  boolean latch;
};
