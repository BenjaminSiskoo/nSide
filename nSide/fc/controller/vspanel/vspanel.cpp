VSPanel::VSPanel(uint port) : Controller(port, (uint)Device::ID::VSPanel) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;

  b1 = b2 = b3 = b4 = 0;
}

auto VSPanel::data() -> uint5 {
  uint data = 0x00;
  if(poll(ServiceButton)) data |= 0x01;
  if(poll(Coin1)) data |= 0x08;
  if(poll(Coin2)) data |= 0x10;
  // data will be left-shifted twice and OR'd into $4016 in CPU
  return data;
}

auto VSPanel::data1() -> uint2 {
  if(counter1 >= 8) return 1;
  if(latched == 1) return 0;

  switch(counter1++) { default: return 0;
  case 2: return b1;
  case 3: return b3;
  }
}

auto VSPanel::data2() -> uint5 {
  if(counter2 >= 8) return 1;
  if(latched == 1) return 0;

  switch(counter2++) { default: return 0;
  case 2: return b2;
  case 3: return b4;
  }
}

auto VSPanel::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter1 = 0;
  counter2 = 0;

  if(latched == 0) {
    b1 = poll(Button1);
    b2 = poll(Button2);
    b3 = poll(Button3);
    b4 = poll(Button4);
  }
}
