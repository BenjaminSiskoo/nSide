Gamepad::Gamepad(bool port, uint index) : Controller(port, index) {
  latched = 0;
  counter = 0;

  b = y = select = start = 0;
  up = down = left = right = 0;
  a = x = l = r = 0;
}

auto Gamepad::data() -> uint2 {
  if(counter >= 16) return 1;
  if(latched == 1) return interface->inputPoll(port, index, B);

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter++) {
  case  0: return b;
  case  1: return y;
  case  2: return select;
  case  3: return start;
  case  4: return up & !down;
  case  5: return down & !up;
  case  6: return left & !right;
  case  7: return right & !left;
  case  8: return a;
  case  9: return x;
  case 10: return l;
  case 11: return r;
  }

  return 0;  //12-15: signature
}

auto Gamepad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b      = interface->inputPoll(port, index, B);
    y      = interface->inputPoll(port, index, Y);
    select = interface->inputPoll(port, index, Select);
    start  = interface->inputPoll(port, index, Start);
    up     = interface->inputPoll(port, index, Up);
    down   = interface->inputPoll(port, index, Down);
    left   = interface->inputPoll(port, index, Left);
    right  = interface->inputPoll(port, index, Right);
    a      = interface->inputPoll(port, index, A);
    x      = interface->inputPoll(port, index, X);
    l      = interface->inputPoll(port, index, L);
    r      = interface->inputPoll(port, index, R);
  }
}
