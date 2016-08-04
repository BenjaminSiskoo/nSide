auto PIA::readIO(uint7 addr, uint8 data) -> uint8 {
  switch(addr & 0x07) {

  case 0x00: {  //SWCHA
    data = (io.swcha & io.swacnt) | (data & ~io.swacnt);
    data |= (Atari2600::peripherals.controllerPort1->direction() << 4) & ~io.swacnt;
    data |= (Atari2600::peripherals.controllerPort2->direction() << 0) & ~io.swacnt;
    break;
  }

  case 0x01: {  //SWACNT
    return io.swacnt;
    break;
  }

  case 0x02: {  //SWCHB
    data = (io.swchb & io.swbcnt) | (data & ~io.swbcnt);
    //Reset switch/button
    if(!io.swbcnt.bit(0)) data.bit(0) = !interface->inputPoll(ID::Port::Hardware, ID::Device::Controls, 1);
    //Select switch/button
    if(!io.swbcnt.bit(1)) data.bit(0) = !interface->inputPoll(ID::Port::Hardware, ID::Device::Controls, 0);
    if(!io.swbcnt.bit(3)) data.bit(0) = io.color;
    if(!io.swbcnt.bit(6)) data.bit(0) = io.difficulty0;
    if(!io.swbcnt.bit(7)) data.bit(0) = io.difficulty1;
    break;
  }

  case 0x03: {  //SWBCNT
    return io.swbcnt;
    break;
  }

  case 0x04: case 0x06: {  //INTIM
    io.timerInterrupt = addr.bit(3);
    break;
  }

  case 0x05: case 0x07: {  //INSTAT
    break;
  }

  }

  return data;
}

auto PIA::writeIO(uint7 addr, uint8 data) -> void {
  switch(addr & 0x17) {

  case 0x00: case 0x10: {
    return;
  }

  case 0x01: case 0x11: {
    io.swacnt = data;
    return;
  }

  case 0x02: case 0x12: {
    return;
  }

  case 0x03: case 0x13: {
    io.swbcnt = data;
    return;
  }

  case 0x04: case 0x05: case 0x06: case 0x07: {
    io.edgeDetect   = addr.bit(0);
    io.pa7Interrupt = addr.bit(1);
    return;
  }

  case 0x14: {  //TIM1T
    io.timer     = data <<  0;
    io.timerMask = 0x000ff;
    io.timerInterrupt = addr.bit(3);
    return;
  }

  case 0x15: {  //TIM8T
    io.timer     = data <<  3;
    io.timerMask = 0x007ff;
    io.timerInterrupt = addr.bit(3);
    return;
  }

  case 0x16: {  //TIM64T
    io.timer     = data <<  6;
    io.timerMask = 0x03fff;
    io.timerInterrupt = addr.bit(3);
    return;
  }

  case 0x17: {  //T1024T
    io.timer     = data << 10;
    io.timerMask = 0x3ffff;
    io.timerInterrupt = addr.bit(3);
    return;
  }

  }

  unreachable;
}
