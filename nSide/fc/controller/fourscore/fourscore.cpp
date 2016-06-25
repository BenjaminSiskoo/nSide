FourScore::FourScore(bool port) : Controller(port) {
  latched = 0;
  counter = 0;
}

auto FourScore::data() -> uint3 {
  if(counter >= 24) return 1;
  if(latched) return interface->inputPoll(port, ID::Device::FourScore, A + 0);

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter++) {
  case  0: return aL;
  case  1: return bL;
  case  2: return selectL;
  case  3: return startL;
  case  4: return (upL & !downL);
  case  5: return (downL & !upL);
  case  6: return (leftL & !rightL);
  case  7: return (rightL & !leftL);
  case  8: return aR;
  case  9: return bR;
  case 10: return selectR;
  case 11: return startR;
  case 12: return (upR & !downR);
  case 13: return (downR & !upR);
  case 14: return (leftR & !rightR);
  case 15: return (rightR & !leftR);
  case 16: return 0;
  case 17: return 0;
  case 18: return port == 1;
  case 19: return port == 0;
  case 20: return 0;
  case 21: return 0;
  case 22: return 0;
  case 23: return 0;
  }
}

auto FourScore::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    auto id = ID::Device::FourScore;
    aL      = interface->inputPoll(port, id, A      + 0);
    bL      = interface->inputPoll(port, id, B      + 0);
    selectL = interface->inputPoll(port, id, Select + 0);
    startL  = interface->inputPoll(port, id, Start  + 0);
    upL     = interface->inputPoll(port, id, Up     + 0);
    downL   = interface->inputPoll(port, id, Down   + 0);
    leftL   = interface->inputPoll(port, id, Left   + 0);
    rightL  = interface->inputPoll(port, id, Right  + 0);
    aR      = interface->inputPoll(port, id, A      + 8);
    bR      = interface->inputPoll(port, id, B      + 8);
    selectR = interface->inputPoll(port, id, Select + 8);
    startR  = interface->inputPoll(port, id, Start  + 8);
    upR     = interface->inputPoll(port, id, Up     + 8);
    downR   = interface->inputPoll(port, id, Down   + 8);
    leftR   = interface->inputPoll(port, id, Left   + 8);
    rightR  = interface->inputPoll(port, id, Right  + 8);
  }
}
