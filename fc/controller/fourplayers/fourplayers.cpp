#ifdef CONTROLLER_CPP

uint2 FourPlayers::data1() {
  if(counter1 >= 8) return 2;
  if(latched) return poll(0) << 1;

  return poll(counter1++) << 1;
}

uint5 FourPlayers::data2() {
  if(counter2 >= 8) return 2;
  if(latched) return poll(8) << 1;

  return poll((counter2++) + 8) << 1;
}

void FourPlayers::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter1 = 0;
  counter2 = 0;
}

FourPlayers::FourPlayers(unsigned port):
Controller(port, (unsigned)Input::Device::FourPlayers) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;
}

#endif
