SFCGamepad::SFCGamepad(uint index) : Expansion(index) {
  latched = 0;
  counter = 0;

  b = y = select = start = 0;
  up = down = left = right = 0;
  a = x = l = r = 0;
}

auto SFCGamepad::data1() -> bool {
  if(counter >= 16) return 1;
  if(latched == 1) return interface->inputPoll(Port::Expansion, index, 0);

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

auto SFCGamepad::data2() -> uint5 {
  return 0;
}

auto SFCGamepad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b      = interface->inputPoll(Port::Expansion, index, B);
    y      = interface->inputPoll(Port::Expansion, index, Y);
    select = interface->inputPoll(Port::Expansion, index, Select);
    start  = interface->inputPoll(Port::Expansion, index, Start);
    up     = interface->inputPoll(Port::Expansion, index, Up);
    down   = interface->inputPoll(Port::Expansion, index, Down);
    left   = interface->inputPoll(Port::Expansion, index, Left);
    right  = interface->inputPoll(Port::Expansion, index, Right);
    a      = interface->inputPoll(Port::Expansion, index, A);
    x      = interface->inputPoll(Port::Expansion, index, X);
    l      = interface->inputPoll(Port::Expansion, index, L);
    r      = interface->inputPoll(Port::Expansion, index, R);
  }
}
