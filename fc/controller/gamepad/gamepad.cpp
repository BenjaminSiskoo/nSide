#ifdef CONTROLLER_CPP

uint5 Gamepad::data() {
  if(counter >= 8) return 1;
  if(latched == 1) return poll((unsigned)Input::JoypadID::A);

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

uint2 Gamepad::data1() {
  return ((data() << 1) & 0x02);
}

uint5 Gamepad::data2() {
  return 0;
}

void Gamepad::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    a      = poll(0);
    b      = poll(1);
    if(!system.vs()) {
      select = poll(2);
      start  = poll(3);
    } else {
      select = 0;
      start  = 0;
    }
    up     = poll(4);
    down   = poll(5);
    left   = poll(6);
    right  = poll(7);
  }
}

Gamepad::Gamepad(unsigned port):
Controller(port, (unsigned)Input::Device::Joypad) {
  latched = 0;
  counter = 0;

  a = b = select = start = 0;
  up = down = left = right = 0;
}

#endif
