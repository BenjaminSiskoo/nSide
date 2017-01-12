SG1000Controls::SG1000Controls(uint port) : Controller(port) {
}

auto SG1000Controls::readData() -> uint8 {
  uint8 data = 0xff;
  data.bit(0) = !platform->inputPoll(port, ID::Device::SG1000Controls, Pause);
  return data;
}
