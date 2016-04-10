SFCGamepad::SFCGamepad(uint port) : Controller(port, Device::SFCGamepad) {
  latched = 0;
  counter = 0;

  b = y = select = start = 0;
  up = down = left = right = 0;
  a = x = l = r = 0;
}

auto SFCGamepad::data() -> uint5 {
  if(counter >= 16) return 1;
  if(latched == 1) return poll(0);

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

auto SFCGamepad::data1() -> uint2 {
  return ((data() << 1) & 0x02);
}

auto SFCGamepad::data2() -> uint5 {
  return 0;
}

auto SFCGamepad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b      = poll(B);
    y      = poll(Y);
    select = poll(Select);
    start  = poll(Start);
    up     = poll(Up);
    down   = poll(Down);
    left   = poll(Left);
    right  = poll(Right);
    a      = poll(A);
    x      = poll(X);
    l      = poll(L);
    r      = poll(R);
  }
}
