#ifdef CONTROLLER_CPP

uint5 SFCGamepad::data() {
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

uint2 SFCGamepad::data1() {
  return ((data() << 1) & 0x02);
}

uint5 SFCGamepad::data2() {
  return 0;
}

void SFCGamepad::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b      = poll( 0);
    y      = poll( 1);
    select = poll( 2);
    start  = poll( 3);
    up     = poll( 4);
    down   = poll( 5);
    left   = poll( 6);
    right  = poll( 7);
    a      = poll( 8);
    x      = poll( 9);
    l      = poll(10);
    r      = poll(11);
  }
}

SFCGamepad::SFCGamepad(unsigned port):
Controller(port, (unsigned)Input::Device::SFCJoypad) {
  latched = 0;
  counter = 0;

  b = y = select = start = 0;
  up = down = left = right = 0;
  a = x = l = r = 0;
}

#endif
