Gamepad::Gamepad(bool port) : Controller(port) {
}

auto Gamepad::readData() -> uint7 {
  uint6 data;

  data.bit(0) = interface->inputPoll(port, ID::Device::Gamepad, Up);
  data.bit(1) = interface->inputPoll(port, ID::Device::Gamepad, Down);
  data.bit(2) = interface->inputPoll(port, ID::Device::Gamepad, Left);
  data.bit(3) = interface->inputPoll(port, ID::Device::Gamepad, Right);
  data.bit(4) = interface->inputPoll(port, ID::Device::Gamepad, I);
  data.bit(5) = interface->inputPoll(port, ID::Device::Gamepad, II);

  data = ~data;
  return latch << 6 | data;
}

auto Gamepad::writeData(uint7 data) -> void {
  latch  = data.bit(6);
}
