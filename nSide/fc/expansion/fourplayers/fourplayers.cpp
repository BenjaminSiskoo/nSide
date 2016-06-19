FourPlayers::FourPlayers(uint index) : Expansion(index) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;
}

auto FourPlayers::data1() -> bool {
  if(counter1 >= 8) return 1;
  if(latched) return interface->inputPoll(Port::Expansion, index, A + 0);

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter1++) {
  case 0: return a3;
  case 1: return b3;
  case 2: return select3;
  case 3: return start3;
  case 4: return up3 & !down3;
  case 5: return down3 & !up3;
  case 6: return left3 & !right3;
  case 7: return right3 & !left3;
  }
}

auto FourPlayers::data2() -> uint5 {
  if(counter2 >= 8) return 2;
  if(latched) return interface->inputPoll(Port::Expansion, index, A + 8) << 1;

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
    a3      = interface->inputPoll(Port::Expansion, index, A      + 0);
    b3      = interface->inputPoll(Port::Expansion, index, B      + 0);
    select3 = interface->inputPoll(Port::Expansion, index, Select + 0);
    start3  = interface->inputPoll(Port::Expansion, index, Start  + 0);
    up3     = interface->inputPoll(Port::Expansion, index, Up     + 0);
    down3   = interface->inputPoll(Port::Expansion, index, Down   + 0);
    left3   = interface->inputPoll(Port::Expansion, index, Left   + 0);
    right3  = interface->inputPoll(Port::Expansion, index, Right  + 0);
    a4      = interface->inputPoll(Port::Expansion, index, A      + 8);
    b4      = interface->inputPoll(Port::Expansion, index, B      + 8);
    select4 = interface->inputPoll(Port::Expansion, index, Select + 8);
    start4  = interface->inputPoll(Port::Expansion, index, Start  + 8);
    up4     = interface->inputPoll(Port::Expansion, index, Up     + 8);
    down4   = interface->inputPoll(Port::Expansion, index, Down   + 8);
    left4   = interface->inputPoll(Port::Expansion, index, Left   + 8);
    right4  = interface->inputPoll(Port::Expansion, index, Right  + 8);
  }
}
