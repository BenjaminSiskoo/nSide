FightingPad6B::FightingPad6B(uint port) : Controller(port) {
  create(Controller::Enter, 1'000'000.0);
  select = 1;
  counter = 0;
  timeout = 0;
}

auto FightingPad6B::main() -> void {
  if(timeout) timeout -= 1;
  else        counter  = 0;
  step(1);
  synchronize(cpu);
}

auto FightingPad6B::readData() -> uint8 {
  uint6 data;

  if(select == 0) {
    if(counter != 2 && counter != 3) {
      data.bit(0) = platform->inputPoll(port, ID::Device::FightingPad6B, Up);
      data.bit(1) = platform->inputPoll(port, ID::Device::FightingPad6B, Down);
      data.bits(2,3) = ~0;
    } else if(counter == 2) {
      data.bits(0,3) = ~0;  //needed for controller detection
    } else if(counter == 3) {
      data.bits(0,3) =  0;
    }
    data.bit(4) = platform->inputPoll(port, ID::Device::FightingPad6B, A);
    data.bit(5) = platform->inputPoll(port, ID::Device::FightingPad6B, Start);
  } else {
    if(counter != 3) {
      data.bit(0) = platform->inputPoll(port, ID::Device::FightingPad6B, Up);
      data.bit(1) = platform->inputPoll(port, ID::Device::FightingPad6B, Down);
      data.bit(2) = platform->inputPoll(port, ID::Device::FightingPad6B, Left);
      data.bit(3) = platform->inputPoll(port, ID::Device::FightingPad6B, Right);
      data.bit(4) = platform->inputPoll(port, ID::Device::FightingPad6B, B);
      data.bit(5) = platform->inputPoll(port, ID::Device::FightingPad6B, C);
    } else {
      data.bit(0) = platform->inputPoll(port, ID::Device::FightingPad6B, Z);
      data.bit(1) = platform->inputPoll(port, ID::Device::FightingPad6B, Y);
      data.bit(2) = platform->inputPoll(port, ID::Device::FightingPad6B, X);
      data.bit(3) = platform->inputPoll(port, ID::Device::FightingPad6B, Mode);
      data.bits(4,5) = 0;
    }
  }

  data = ~data;
  return latch << 7 | select << 6 | data;
}

auto FightingPad6B::writeData(uint8 data) -> void {
  if(!select && data.bit(6)) counter = (counter + 1) % 5;
  select = data.bit(6);
  timeout = 1607;

  latch  = data.bit(7);
}
