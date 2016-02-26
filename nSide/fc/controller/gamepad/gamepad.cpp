Gamepad::Gamepad(uint port) : Controller(port, (uint)Device::ID::Gamepad) {
  latched = 0;
  counter = 0;

  a = b = select = start = 0;
  up = down = left = right = 0;
}

auto Gamepad::data() -> uint5 {
  if(counter >= 8) return 1;
  if(latched == 1) return poll(A);

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

auto Gamepad::data1() -> uint2 {
  return ((data() << 1) & 0x02);
}

auto Gamepad::data2() -> uint5 {
  return 0;
}

auto Gamepad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    a      = poll(A);
    b      = poll(B);
    select = poll(Select);
    start  = poll(Start);
    up     = poll(Up);
    down   = poll(Down);
    left   = poll(Left);
    right  = poll(Right);
  }
}
