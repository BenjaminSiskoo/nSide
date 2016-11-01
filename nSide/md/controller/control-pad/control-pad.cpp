ControlPad::ControlPad(uint port) : Controller(port) {
  select = 1;
}

auto ControlPad::readData() -> uint8 {
  uint6 data;

  if(select == 0) {
    data.bit(0) = interface->inputPoll(port, ID::Device::ControlPad, Up);
    data.bit(1) = interface->inputPoll(port, ID::Device::ControlPad, Down);
    data.bit(2) = 1;
    data.bit(3) = 1;
    data.bit(4) = interface->inputPoll(port, ID::Device::ControlPad, A);
    data.bit(5) = interface->inputPoll(port, ID::Device::ControlPad, Start);
  } else {
    data.bit(0) = interface->inputPoll(port, ID::Device::ControlPad, Up);
    data.bit(1) = interface->inputPoll(port, ID::Device::ControlPad, Down);
    data.bit(2) = interface->inputPoll(port, ID::Device::ControlPad, Left);
    data.bit(3) = interface->inputPoll(port, ID::Device::ControlPad, Right);
    data.bit(4) = interface->inputPoll(port, ID::Device::ControlPad, B);
    data.bit(5) = interface->inputPoll(port, ID::Device::ControlPad, C);
  }

  data = ~data;
  return latch << 7 | select << 6 | data;
}

auto ControlPad::writeData(uint8 data) -> void {
  select = data.bit(6);
  latch  = data.bit(7);
}
