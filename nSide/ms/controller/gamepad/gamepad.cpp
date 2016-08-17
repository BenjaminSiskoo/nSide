Gamepad::Gamepad(bool port) : Controller(port) {
}

auto Gamepad::read() -> uint7 {
  bool up    = interface->inputPoll(port, ID::Device::Gamepad, Up);
  bool down  = interface->inputPoll(port, ID::Device::Gamepad, Down);
  bool left  = interface->inputPoll(port, ID::Device::Gamepad, Left);
  bool right = interface->inputPoll(port, ID::Device::Gamepad, Right);
  bool i     = interface->inputPoll(port, ID::Device::Gamepad, I);
  bool ii    = interface->inputPoll(port, ID::Device::Gamepad, II);
  return (
    !up    << 0
  | !down  << 1
  | !left  << 2
  | !right << 3
  | !i     << 4
  | !ii    << 5
  | 0      << 6
  );
}

auto Gamepad::write(uint7 data) -> void {
}
