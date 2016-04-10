FourScore::FourScore(uint port) : Controller(port, Device::FourScore) {
  latched = 0;
  counter = 0;
}

auto FourScore::data() -> uint5 {
  if(counter >= 24) return 1;
  if(latched) return poll(A);

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
    aL      = poll(A + 0);
    bL      = poll(B + 0);
    selectL = poll(Select + 0);
    startL  = poll(Start + 0);
    upL     = poll(Up + 0);
    downL   = poll(Down + 0);
    leftL   = poll(Left + 0);
    rightL  = poll(Right + 0);
    aR      = poll(A + 8);
    bR      = poll(B + 8);
    selectR = poll(Select + 8);
    startR  = poll(Start + 8);
    upR     = poll(Up + 8);
    downR   = poll(Down + 8);
    leftR   = poll(Left + 8);
    rightR  = poll(Right + 8);
  }
}
