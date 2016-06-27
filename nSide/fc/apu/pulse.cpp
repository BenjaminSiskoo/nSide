auto APU::Pulse::clockLength() -> void {
  if(envelope.loopMode == 0) {
    if(lengthCounter) lengthCounter--;
  }
}

auto APU::Pulse::clock() -> uint8 {
  if(!sweep.checkPeriod()) return 0;
  if(lengthCounter == 0) return 0;

  static const uint dutyTable[] = {1, 2, 4, 6};
  static const uint umcDutyTable[] = {1, 4, 2, 6};
  uint dutyLimit = apu.version != APU::Version::UA6527P ? dutyTable[duty] : umcDutyTable[duty];
  uint8 result = (dutyCounter < dutyLimit) ? envelope.volume() : 0;
  if(sweep.pulsePeriod < 0x008) result = 0;

  if(--periodCounter == 0) {
    periodCounter = (sweep.pulsePeriod + 1) * 2;
    dutyCounter++;
  }

  return result;
}

auto APU::Pulse::power() -> void {
  envelope.power();
  sweep.power();
}

auto APU::Pulse::reset() -> void {
  envelope.reset();
  sweep.reset();

  lengthCounter = 0;

  duty = 0;
  dutyCounter = 0;
  period = 0;
  periodCounter = 1;
}
