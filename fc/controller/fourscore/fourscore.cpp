FourScore::FourScore(uint port) : Controller(port, (uint)Device::ID::FourScore) {
  latched = 0;
  counter = 0;
}

auto FourScore::data() -> uint5 {
  if(counter >= 24) return 1;
  if(latched) return poll(0);
  uint5 data = 0;

  if(counter >= 16) data = counter == (19 - port);
  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  else switch(counter++) {
  case  0: return aL << 1;
  case  1: return bL << 1;
  case  2: return selectL << 1;
  case  3: return startL << 1;
  case  4: return (upL & !downL) << 1;
  case  5: return (downL & !upL) << 1;
  case  6: return (leftL & !rightL) << 1;
  case  7: return (rightL & !leftL) << 1;
  case  8: return aR << 1;
  case  9: return bR << 1;
  case 10: return selectR << 1;
  case 11: return startR << 1;
  case 12: return (upR & !downR) << 1;
  case 13: return (downR & !upR) << 1;
  case 14: return (leftR & !rightR) << 1;
  case 15: return (rightR & !leftR) << 1;
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
