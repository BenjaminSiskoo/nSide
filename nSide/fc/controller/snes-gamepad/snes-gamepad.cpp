SNESGamepad::SNESGamepad(uint port) : Controller(port) {
  latched = 0;
  counter = 0;
}

auto SNESGamepad::data() -> uint3 {
  if(counter >= 16) return 1;
  if(latched == 1) return platform->inputPoll(port, ID::Device::SNESGamepad, B);

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

auto SNESGamepad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b      = platform->inputPoll(port, ID::Device::SNESGamepad, B);
    y      = platform->inputPoll(port, ID::Device::SNESGamepad, Y);
    select = platform->inputPoll(port, ID::Device::SNESGamepad, Select);
    start  = platform->inputPoll(port, ID::Device::SNESGamepad, Start);
    up     = platform->inputPoll(port, ID::Device::SNESGamepad, Up);
    down   = platform->inputPoll(port, ID::Device::SNESGamepad, Down);
    left   = platform->inputPoll(port, ID::Device::SNESGamepad, Left);
    right  = platform->inputPoll(port, ID::Device::SNESGamepad, Right);
    a      = platform->inputPoll(port, ID::Device::SNESGamepad, A);
    x      = platform->inputPoll(port, ID::Device::SNESGamepad, X);
    l      = platform->inputPoll(port, ID::Device::SNESGamepad, L);
    r      = platform->inputPoll(port, ID::Device::SNESGamepad, R);
  }
}
