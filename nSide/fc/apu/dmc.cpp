auto APU::DMC::start() -> void {
  if(lengthCounter == 0) {
    readAddress = 0x4000 + (addrLatch << 6);
    lengthCounter = (lengthLatch << 4) + 1;
  }
}

auto APU::DMC::stop() -> void {
  lengthCounter = 0;
  dmaDelayCounter = 0;
  cpu.setRDYLine(1);
  cpu.setRDYAddress(false);
}

auto APU::DMC::clock() -> uint8 {
  uint8 result = dacLatch;

  if(dmaDelayCounter > 0) {
    dmaDelayCounter--;

    if(dmaDelayCounter == 1) {
      cpu.setRDYAddress(true, 0x8000 | readAddress);
    } else if(dmaDelayCounter == 0) {
      cpu.setRDYLine(1);
      cpu.setRDYAddress(false);

      dmaBuffer = cpu.mdr();
      haveDMABuffer = true;
      lengthCounter--;
      readAddress++;

      if(lengthCounter == 0) {
        if(loopMode) {
          start();
        } else if(irqEnable) {
          irqPending = true;
          apu.setIRQLine();
        }
      }
    }
  }

  if(--periodCounter == 0) {
    if(haveSample) {
      int delta = (((sample >> bitCounter) & 1) << 2) - 2;
      uint data = dacLatch + delta;
      if((data & 0x80) == 0) dacLatch = data;
    }

    if(++bitCounter == 0) {
      if(haveDMABuffer) {
        haveSample = true;
        sample = dmaBuffer;
        haveDMABuffer = false;
      } else {
        haveSample = false;
      }
    }

    if(system.region() == System::Region::NTSC)
      periodCounter = ntscDMCPeriodTable[period];
    else
      periodCounter = palDMCPeriodTable[period];
  }

  if(lengthCounter > 0 && haveDMABuffer == false && dmaDelayCounter == 0) {
    cpu.setRDYLine(0);
    dmaDelayCounter = 4;
  }

  return result;
}

auto APU::DMC::power() -> void {
}

auto APU::DMC::reset() -> void {
  lengthCounter = 0;
  irqPending = 0;

  period = 0;
  if(system.region() == System::Region::NTSC)
    periodCounter = ntscDMCPeriodTable[0];
  else
    periodCounter = palDMCPeriodTable[0];
  irqEnable = 0;
  loopMode = 0;
  dacLatch = 0;
  addrLatch = 0;
  lengthLatch = 0;
  readAddress = 0;
  dmaDelayCounter = 0;
  bitCounter = 0;
  haveDMABuffer = 0;
  dmaBuffer = 0;
  haveSample = 0;
  sample = 0;
}

auto APU::DMC::serialize(serializer& s) -> void {
  s.integer(lengthCounter);
  s.integer(irqPending);

  s.integer(period);
  s.integer(periodCounter);

  s.integer(irqEnable);
  s.integer(loopMode);

  s.integer(dacLatch);
  s.integer(addrLatch);
  s.integer(lengthLatch);

  s.integer(readAddress);
  s.integer(dmaDelayCounter);

  s.integer(bitCounter);
  s.integer(haveDMABuffer);
  s.integer(dmaBuffer);

  s.integer(haveSample);
  s.integer(sample);
}
