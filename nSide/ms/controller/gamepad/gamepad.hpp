struct Gamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, I, II,
  };

  Gamepad(bool port);

  auto read() -> uint7;
  auto write(uint7 data) -> void;
};
