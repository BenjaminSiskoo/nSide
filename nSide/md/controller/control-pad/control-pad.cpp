ControlPad::ControlPad(bool port) : Controller(port) {
  th = 1;
}

auto ControlPad::read() -> uint7 {
  bool up   = interface->inputPoll(port, ID::Device::ControlPad, Up);
  bool down = interface->inputPoll(port, ID::Device::ControlPad, Down);
  if(th) {
    bool left  = interface->inputPoll(port, ID::Device::ControlPad, Left);
    bool right = interface->inputPoll(port, ID::Device::ControlPad, Right);
    bool b     = interface->inputPoll(port, ID::Device::ControlPad, B);
    bool c     = interface->inputPoll(port, ID::Device::ControlPad, C);
    return (
      !up    << 0
    | !down  << 1
    | !left  << 2
    | !right << 3
    | !b     << 4
    | !c     << 5
    | th     << 6
    );
  } else {
    bool a     = interface->inputPoll(port, ID::Device::ControlPad, A);
    bool start = interface->inputPoll(port, ID::Device::ControlPad, Start);
    return (
      !up    << 0
    | !down  << 1
    | 0      << 2
    | 0      << 3
    | !a     << 4
    | !start << 5
    | th     << 6
    );
  }
  unreachable;
}

auto ControlPad::write(uint7 data) -> void {
  th = data.bit(6);
}
