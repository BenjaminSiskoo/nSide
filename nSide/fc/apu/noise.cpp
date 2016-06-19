auto APU::Noise::clockLength() -> void {
  if(envelope.loopMode == 0) {
    if(lengthCounter > 0) lengthCounter--;
  }
}

auto APU::Noise::clock() -> uint8 {
  if(lengthCounter == 0) return 0;

  uint8 result = (lfsr & 1) ? envelope.volume() : 0;

  if(--periodCounter == 0) {
    uint feedback;

    //TODO: Check if the RP2A03E and prior revisions support short mode.
    if(shortMode && apu.revision != APU::Revision::RP2A03 && apu.revision != APU::Revision::RP2A03F) {
      feedback = ((lfsr >> 0) & 1) ^ ((lfsr >> 6) & 1);
    } else {
      feedback = ((lfsr >> 0) & 1) ^ ((lfsr >> 1) & 1);
    }

    lfsr = (lfsr >> 1) | (feedback << 14);
    periodCounter = system.region() == System::Region::NTSC ? apu.ntscNoisePeriodTable[period] : apu.palNoisePeriodTable[period];
  }

  return result;
}

auto APU::Noise::power() -> void {
}

auto APU::Noise::reset() -> void {
  lengthCounter = 0;

  envelope.speed = 0;
  envelope.useSpeedAsVolume = 0;
  envelope.loopMode = 0;
  envelope.reloadDecay = 0;
  envelope.decayCounter = 0;
  envelope.decayVolume = 0;

  period = 0;
  periodCounter = 1;
  shortMode = 0;
  lfsr = 1;
}

auto APU::Noise::serialize(serializer& s) -> void {
  s.integer(lengthCounter);

  envelope.serialize(s);

  s.integer(period);
  s.integer(periodCounter);

  s.integer(shortMode);
  s.integer(lfsr);
}

const uint16 APU::ntscNoisePeriodTable[16] = {
  4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
};

const uint16 APU::palNoisePeriodTable[16] = {
  4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778,
};
