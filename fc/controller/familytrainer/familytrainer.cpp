#ifdef CONTROLLER_CPP

uint5 FamilyTrainer::data() {
  if(counter >= 8) return 0x18;
  if(latched == 1) return (b2 << 3) | (b4 << 4);

  switch(counter++) {
  case  0: return (b2  << 3) | (b4  << 4);
  case  1: return (b1  << 3) | (b3  << 4);
  case  2: return (b5  << 3) | (b12 << 4);
  case  3: return (b9  << 3) | (b8  << 4);
  case  4: return (b6  << 3) | 0x10;
  case  5: return (b10 << 3) | 0x10;
  case  6: return (b11 << 3) | 0x10;
  case  7: return (b7  << 3) | 0x10;
  }
}

uint2 FamilyTrainer::data1() {
  return 0;
}

uint5 FamilyTrainer::data2() {
  return data();
}

void FamilyTrainer::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b1  = poll( 0);
    b2  = poll( 1);
    b3  = poll( 2);
    b4  = poll( 3);
    b5  = poll( 4);
    b6  = poll( 5);
    b7  = poll( 6);
    b8  = poll( 7);
    b9  = poll( 8);
    b10 = poll( 9);
    b11 = poll(10);
    b12 = poll(11);
  }
}

FamilyTrainer::FamilyTrainer(unsigned port):
Controller(port, (unsigned)Input::Device::FamilyTrainer) {
  latched = 0;
  counter = 0;

  b1 = b2 = b3 = b4 = 0;
  b5 = b6 = b7 = b8 = 0;
  b9 = b10 = b11 = b12 = 0;
}

#endif
