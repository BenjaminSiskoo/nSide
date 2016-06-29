GamepadMic::GamepadMic(bool port) : Controller(port) {
  latched = 0;
  counter = 0;
}

auto GamepadMic::data() -> uint3 {
  if(counter >= 8) return 1;
  if(latched == 1) return interface->inputPoll(port, ID::Device::GamepadMic, A);

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter++) {
  case 0: return a;
  case 1: return b;
  case 2: return 0;
  case 3: return 0;
  case 4: return up & !down;
  case 5: return down & !up;
  case 6: return left & !right;
  case 7: return right & !left;
  }
  unreachable;
}

auto GamepadMic::mic() -> bool {
  return interface->inputPoll(port, ID::Device::GamepadMic, Mic) && random() & 1;
}

auto GamepadMic::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    a      = interface->inputPoll(port, ID::Device::GamepadMic, A);
    b      = interface->inputPoll(port, ID::Device::GamepadMic, B);
    up     = interface->inputPoll(port, ID::Device::GamepadMic, Up);
    down   = interface->inputPoll(port, ID::Device::GamepadMic, Down);
    left   = interface->inputPoll(port, ID::Device::GamepadMic, Left);
    right  = interface->inputPoll(port, ID::Device::GamepadMic, Right);
  }
}
