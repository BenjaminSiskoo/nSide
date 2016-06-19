Gamepad::Gamepad(bool port, uint index) : Controller(port, index) {
  latched = 0;
  counter = 0;

  a = b = select = start = 0;
  up = down = left = right = 0;
}

auto Gamepad::data() -> uint3 {
  if(counter >= 8) return 1;
  if(latched == 1) return interface->inputPoll(port, index, A);

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter++) {
  case 0: return a;
  case 1: return b;
  case 2: return select;
  case 3: return start;
  case 4: return up & !down;
  case 5: return down & !up;
  case 6: return left & !right;
  case 7: return right & !left;
  }
}

auto Gamepad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    a      = interface->inputPoll(port, index, A);
    b      = interface->inputPoll(port, index, B);
    select = interface->inputPoll(port, index, Select);
    start  = interface->inputPoll(port, index, Start);
    up     = interface->inputPoll(port, index, Up);
    down   = interface->inputPoll(port, index, Down);
    left   = interface->inputPoll(port, index, Left);
    right  = interface->inputPoll(port, index, Right);
  }
}
