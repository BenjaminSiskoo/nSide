auto VSSystem::resetButtons() -> void {
  mainControlLatched = 0;
  mainControlCounter1 = 0;
  mainControlCounter2 = 0;

  subControlLatched = 0;
  subControlCounter1 = 0;
  subControlCounter2 = 0;

  for(bool& button : mainButtons) button = false;
  for(bool& button : subButtons) button = false;
}

auto VSSystem::poll(bool side, uint input) -> int16 {
  return interface->inputPoll(Port::Arcade, Peripheral::ArcadePanel::VSPanel, input);
}

auto VSSystem::data1(bool side) -> bool {
  uint& counter = side == 0 ? mainControlCounter1 : subControlCounter1;
  bool* buttons = side == 0 ? mainButtons : subButtons;
  bool data;
  if(!swapControllers) data = peripherals.controllerPort1->data() & 0x01;
  else                 data = peripherals.controllerPort2->data() & 0x01;
  if(counter == 2) return buttons[Button1];
  if(counter == 3) return buttons[Button3];
  return data;
}

auto VSSystem::data2(bool side) -> bool {
  uint& counter = side == 0 ? mainControlCounter2 : subControlCounter2;
  bool* buttons = side == 0 ? mainButtons : subButtons;
  bool data;
  if(!swapControllers) data = peripherals.controllerPort2->data() & 0x01;
  else                 data = peripherals.controllerPort1->data() & 0x01;
  if(counter == 2) return buttons[Button2];
  if(counter == 3) return buttons[Button4];
  return data;
}

auto VSSystem::latch(bool side, bool data) -> void {
  if(side == 0) {
    if(mainControlLatched == data) return;
    mainControlLatched = data;
    mainControlCounter1 = 0;
    mainControlCounter2 = 0;

    if(mainControlLatched == 0) {
      for(uint i : {Button1, Button2, Button3, Button4}) mainButtons[i] = poll(0, i);
    }
  } else {
    if(subControlLatched == data) return;
    subControlLatched = data;
    subControlCounter1 = 0;
    subControlCounter2 = 0;

    if(subControlLatched == 0) {
      for(uint i : {Button1, Button2, Button3, Button4}) subButtons[i] = poll(1, i);
    }
  }
}
