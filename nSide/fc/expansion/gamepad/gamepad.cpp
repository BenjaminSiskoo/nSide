GamepadE::GamepadE(uint index) : Expansion(index) {
  latched = 0;
  counter = 0;

  a = b = select = start = 0;
  up = down = left = right = 0;
}

auto GamepadE::data1() -> bool {
  if(counter >= 8) return 1;
  if(latched == 1) return interface->inputPoll(Port::Expansion, index, A);

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

auto GamepadE::data2() -> uint5 {
  return 0;
}

auto GamepadE::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    a      = interface->inputPoll(Port::Expansion, index, A);
    b      = interface->inputPoll(Port::Expansion, index, B);
    select = interface->inputPoll(Port::Expansion, index, Select);
    start  = interface->inputPoll(Port::Expansion, index, Start);
    up     = interface->inputPoll(Port::Expansion, index, Up);
    down   = interface->inputPoll(Port::Expansion, index, Down);
    left   = interface->inputPoll(Port::Expansion, index, Left);
    right  = interface->inputPoll(Port::Expansion, index, Right);
  }
}
