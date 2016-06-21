PowerPad::PowerPad(bool port, uint index) : Controller(port, index) {
  latched = 0;
  counter = 0;

  b1 = b2  = b3  = b4  = 0;
  b5 = b6  = b7  = b8  = 0;
  b9 = b10 = b11 = b12 = 0;
}

auto PowerPad::data() -> uint3 {
  if(counter >= 8) return 0x6;
  if(latched == 1) return (b2 << 1) | (b4 << 2);

  switch(counter++) {
  case  0: return (b2  << 1) | (b4  << 2);
  case  1: return (b1  << 1) | (b3  << 2);
  case  2: return (b5  << 1) | (b12 << 2);
  case  3: return (b9  << 1) | (b8  << 2);
  case  4: return (b6  << 1) | (1   << 2);
  case  5: return (b10 << 1) | (1   << 2);
  case  6: return (b11 << 1) | (1   << 2);
  case  7: return (b7  << 1) | (1   << 2);
  }
}

auto PowerPad::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b1  = interface->inputPoll(port, index,  0);
    b2  = interface->inputPoll(port, index,  1);
    b3  = interface->inputPoll(port, index,  2);
    b4  = interface->inputPoll(port, index,  3);
    b5  = interface->inputPoll(port, index,  4);
    b6  = interface->inputPoll(port, index,  5);
    b7  = interface->inputPoll(port, index,  6);
    b8  = interface->inputPoll(port, index,  7);
    b9  = interface->inputPoll(port, index,  8);
    b10 = interface->inputPoll(port, index,  9);
    b11 = interface->inputPoll(port, index, 10);
    b12 = interface->inputPoll(port, index, 11);
  }
}
