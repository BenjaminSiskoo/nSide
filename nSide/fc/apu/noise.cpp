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

    //TODO: Check if the RP2A03E and prior versions support short mode.
    if(shortMode && apu.version != APU::Version::RP2A03 && apu.version != APU::Version::RP2A03F) {
      feedback = ((lfsr >> 0) & 1) ^ ((lfsr >> 6) & 1);
    } else {
      feedback = ((lfsr >> 0) & 1) ^ ((lfsr >> 1) & 1);
    }

    lfsr = (lfsr >> 1) | (feedback << 14);
    periodCounter = system.region() != System::Region::PAL ? apu.noisePeriodTableNTSC[period] : apu.noisePeriodTablePAL[period];
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
