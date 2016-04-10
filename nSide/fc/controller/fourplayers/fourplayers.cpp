FourPlayers::FourPlayers(uint port) : Controller(port, Device::FourPlayers) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;
}

auto FourPlayers::data1() -> uint2 {
  if(counter1 >= 8) return 2;
  if(latched) return poll(A + 0) << 1;

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter1++) {
  case 0: return a3 << 1;
  case 1: return b3 << 1;
  case 2: return select3 << 1;
  case 3: return start3 << 1;
  case 4: return (up3 & !down3) << 1;
  case 5: return (down3 & !up3) << 1;
  case 6: return (left3 & !right3) << 1;
  case 7: return (right3 & !left3) << 1;
  }
}

auto FourPlayers::data2() -> uint5 {
  if(counter2 >= 8) return 2;
  if(latched) return poll(A + 8) << 1;

  switch(counter2++) {
  case 0: return a4 << 1;
  case 1: return b4 << 1;
  case 2: return select4 << 1;
  case 3: return start4 << 1;
  case 4: return (up4 & !down4) << 1;
  case 5: return (down4 & !up4) << 1;
  case 6: return (left4 & !right4) << 1;
  case 7: return (right4 & !left4) << 1;
  }
}

auto FourPlayers::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter1 = 0;
  counter2 = 0;

  if(latched == 0) {
    a3      = poll(A + 0);
    b3      = poll(B + 0);
    select3 = poll(Select + 0);
    start3  = poll(Start + 0);
    up3     = poll(Up + 0);
    down3   = poll(Down + 0);
    left3   = poll(Left + 0);
    right3  = poll(Right + 0);
    a4      = poll(A + 8);
    b4      = poll(B + 8);
    select4 = poll(Select + 8);
    start4  = poll(Start + 8);
    up4     = poll(Up + 8);
    down4   = poll(Down + 8);
    left4   = poll(Left + 8);
    right4  = poll(Right + 8);
  }
}
