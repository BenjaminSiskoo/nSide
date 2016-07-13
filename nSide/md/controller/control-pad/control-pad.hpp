struct ControlPad : Controller {
  enum : uint {
    Up, Down, Left, Right, A, B, C, Start,
  };

  ControlPad(bool port);

  auto read() -> uint7;
  auto write(uint7 data) -> void;

private:
  bool th;
};
