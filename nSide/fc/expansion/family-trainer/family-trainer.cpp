FamilyTrainer::FamilyTrainer() {
  latched = 0;
  counter = 0;
}

auto FamilyTrainer::data1() -> bool {
  return 0;
}

auto FamilyTrainer::data2() -> uint5 {
  if(counter >= 8) return 0x18;
  if(latched == 1) return (platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer, 1) << 1) | (platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer, 3) << 2);

  switch(counter++) {
  case 0: return (b2  << 3) | (b4  << 4);
  case 1: return (b1  << 3) | (b3  << 4);
  case 2: return (b5  << 3) | (b12 << 4);
  case 3: return (b9  << 3) | (b8  << 4);
  case 4: return (b6  << 3) | (1   << 4);
  case 5: return (b10 << 3) | (1   << 4);
  case 6: return (b11 << 3) | (1   << 4);
  case 7: return (b7  << 3) | (1   << 4);
  }
  unreachable;
}

auto FamilyTrainer::write(uint3 data) -> void {
  if(latched == data.bit(0)) return;
  latched = data.bit(0);
  counter = 0;

  if(latched == 0) {
    b1  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  0);
    b2  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  1);
    b3  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  2);
    b4  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  3);
    b5  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  4);
    b6  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  5);
    b7  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  6);
    b8  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  7);
    b9  = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  8);
    b10 = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer,  9);
    b11 = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer, 10);
    b12 = platform->inputPoll(ID::Port::Expansion, ID::Device::FamilyTrainer, 11);
  }
}
