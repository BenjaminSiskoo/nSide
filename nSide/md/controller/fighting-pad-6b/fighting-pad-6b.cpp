FightingPad6B::FightingPad6B(bool port) : Controller(port) {
  create(Controller::Enter, system.colorburst() * 15.0 / 7.0);
  th = 1;
  counter = 0;
  timeout = 0;
}

auto FightingPad6B::main() -> void {
  if(timeout) timeout -= 1;
  else        counter = 0;
  step(1);
}

auto FightingPad6B::data() -> uint7 {
  bool up   = interface->inputPoll(port, ID::Device::FightingPad6B, Up);
  bool down = interface->inputPoll(port, ID::Device::FightingPad6B, Down);
  if(th && counter != 3) {
    bool left  = interface->inputPoll(port, ID::Device::FightingPad6B, Left);
    bool right = interface->inputPoll(port, ID::Device::FightingPad6B, Right);
    bool b     = interface->inputPoll(port, ID::Device::FightingPad6B, B);
    bool c     = interface->inputPoll(port, ID::Device::FightingPad6B, C);
    return (
      !up    << 0
    | !down  << 1
    | !left  << 2
    | !right << 3
    | !b     << 4
    | !c     << 5
    | th     << 6
    );
  } else if(!th) {
    bool a     = interface->inputPoll(port, ID::Device::FightingPad6B, A);
    bool start = interface->inputPoll(port, ID::Device::FightingPad6B, Start);
    if(counter != 3 && counter != 4) {
      return (
        !up    << 0
      | !down  << 1
      | 0      << 2
      | 0      << 3
      | !a     << 4
      | !start << 5
      | th     << 6
      );
    } else if(counter == 3) {
      return (
        0      << 0
      | 0      << 1
      | 0      << 2
      | 0      << 3
      | !a     << 4
      | !start << 5
      | th     << 6
      );
    } else if(counter == 4) {
      return (
        1      << 0
      | 1      << 1
      | 1      << 2
      | 1      << 3
      | !a     << 4
      | !start << 5
      | th     << 6
      );
    }
  } else if(th && counter == 3) {
    bool z     = interface->inputPoll(port, ID::Device::FightingPad6B, Z);
    bool y     = interface->inputPoll(port, ID::Device::FightingPad6B, Y);
    bool x     = interface->inputPoll(port, ID::Device::FightingPad6B, X);
    bool mode  = interface->inputPoll(port, ID::Device::FightingPad6B, Mode);
    return (
      !z     << 0
    | !y     << 1
    | !x     << 2
    | !mode  << 3
    | 1      << 4
    | 1      << 5
    | th     << 6
    );
  }
  unreachable;
}

auto ControlPad::write(uint7 data) -> void {
  if(th && !data.bit(6)) counter++;
  th = data.bit(6);
  timeout = 8192;
}
