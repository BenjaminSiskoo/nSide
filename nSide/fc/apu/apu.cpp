#include <fc/fc.hpp>

namespace Famicom {

APU apu;

#include "envelope.cpp"
#include "sweep.cpp"
#include "pulse.cpp"
#include "triangle.cpp"
#include "noise.cpp"
#include "dmc.cpp"
#include "serialization.cpp"

APU::APU() {
  for(uint amplitude : range(32)) {
    if(amplitude == 0) {
      pulseDAC[amplitude] = 0;
    } else {
      pulseDAC[amplitude] = 16384.0 * 95.88 / (8128.0 / amplitude + 100.0);
    }
  }

  for(uint dmcAmplitude : range(128)) {
    for(uint triangleAmplitude : range(16)) {
      for(uint noiseAmplitude : range(16)) {
        if(dmcAmplitude == 0 && triangleAmplitude == 0 && noiseAmplitude == 0) {
          dmcTriangleNoiseDAC[dmcAmplitude][triangleAmplitude][noiseAmplitude] = 0;
        } else {
          dmcTriangleNoiseDAC[dmcAmplitude][triangleAmplitude][noiseAmplitude]
          = 16384.0 * 159.79 / (100.0 + 1.0 / (triangleAmplitude / 8227.0 + noiseAmplitude / 12241.0 + dmcAmplitude / 22638.0));
        }
      }
    }
  }
}

auto APU::Enter() -> void {
  while(true) scheduler.synchronize(), apu.main();
}

auto APU::main() -> void {
  uint pulseOutput, triangleOutput, noiseOutput, dmcOutput;

  pulseOutput  = pulse[0].clock();
  pulseOutput += pulse[1].clock();
  triangleOutput = triangle.clock();
  noiseOutput = noise.clock();
  dmcOutput = dmc.clock();

  clockFrameCounterDivider();

  int output = pulseDAC[pulseOutput] + dmcTriangleNoiseDAC[dmcOutput][triangleOutput][noiseOutput];

  output  = filter.runHipassStrong(output);
  output += cartridgeSample;
  output  = filter.runHipassWeak(output);
//output  = filter.runLopass(output);
  output  = sclamp<16>(output);

  stream->sample(output / 32768.0);

  tick();
}

auto APU::tick() -> void {
  switch(system.region()) {
  case System::Region::NTSC:  clock += 12; break;
  case System::Region::PAL:   clock += 16; break;
  case System::Region::Dendy: clock += 15; break;
  }
  if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
}

auto APU::irqLine() -> void {
  cpu.apuLine(frame.irqPending || dmc.irqPending);
}

auto APU::setSample(int16 sample) -> void {
  cartridgeSample = sample;
}

auto APU::power() -> void {
  filter.hipassStrong = 0;
  filter.hipassWeak = 0;
  filter.lopass = 0;

  pulse[0].power();
  pulse[1].power();
  triangle.power();
  noise.power();
  dmc.power();
}

auto APU::reset() -> void {
  create(APU::Enter, system.cpuFrequency());
  double clockDivider;
  switch(system.region()) {
  case System::Region::NTSC:  clockDivider = 12.0; break;
  case System::Region::PAL:   clockDivider = 16.0; break;
  case System::Region::Dendy: clockDivider = 15.0; break;
  }
  stream = Emulator::audio.createStream(1, system.cpuFrequency() / clockDivider);

  pulse[0].reset();
  pulse[1].reset();
  triangle.reset();
  noise.reset();
  dmc.reset();

  frame.irqPending = 0;

  frame.mode = 0;
  frame.counter = 0;
  frame.divider = 1;

  enabledChannels = 0;
  cartridgeSample = 0;

  irqLine();
}

auto APU::readIO(uint16 addr) -> uint8 {
  if(addr == 0x4015) {
    uint8 result = 0x00;
    result |= pulse[0].lengthCounter ? 0x01 : 0;
    result |= pulse[1].lengthCounter ? 0x02 : 0;
    result |= triangle.lengthCounter ? 0x04 : 0;
    result |=    noise.lengthCounter ? 0x08 : 0;
    result |=      dmc.lengthCounter ? 0x10 : 0;
    result |=       frame.irqPending ? 0x40 : 0;
    result |=         dmc.irqPending ? 0x80 : 0;

    frame.irqPending = false;
    irqLine();

    return result;
  }

  return cpu.mdr();
}

auto APU::writeIO(uint16 addr, uint8 data) -> void {
  const uint n = addr.bit(2);  //pulse#

  switch(addr) {
  case 0x4000: case 0x4004: {
    pulse[n].duty = data.bits(6,7);
    pulse[n].envelope.loopMode = data.bit(5);
    pulse[n].envelope.useSpeedAsVolume = data.bit(4);
    pulse[n].envelope.speed = data.bits(0,3);
    break;
  }

  case 0x4001: case 0x4005: {
    pulse[n].sweep.enable = data.bit(7);
    pulse[n].sweep.period = data.bits(4,6);
    pulse[n].sweep.decrement = data.bit(3);
    pulse[n].sweep.shift = data.bits(0,2);
    pulse[n].sweep.reload = true;
    break;
  }

  case 0x4002: case 0x4006: {
    pulse[n].period = (pulse[n].period & 0x0700) | (data << 0);
    pulse[n].sweep.pulsePeriod = (pulse[n].sweep.pulsePeriod & 0x0700) | (data << 0);
    break;
  }

  case 0x4003: case 0x4007: {
    pulse[n].period = (pulse[n].period & 0x00ff) | (data << 8);
    pulse[n].sweep.pulsePeriod = (pulse[n].sweep.pulsePeriod & 0x00ff) | (data << 8);

    pulse[n].dutyCounter = 7;
    pulse[n].envelope.reloadDecay = true;

    if(enabledChannels & (1 << n)) {
      pulse[n].lengthCounter = lengthCounterTable[data.bits(3,7)];
    }
    break;
  }

  case 0x4008: {
    triangle.haltLengthCounter = data.bit(7);
    triangle.linearLength = data.bits(0,6);
    break;
  }

  case 0x400a: {
    triangle.period = (triangle.period & 0x0700) | (data << 0);
    break;
  }

  case 0x400b: {
    triangle.period = (triangle.period & 0x00ff) | (data << 8);

    triangle.reloadLinear = true;

    if(enabledChannels.bit(2)) triangle.lengthCounter = lengthCounterTable[data.bits(3,7)];
    break;
  }

  case 0x400c: {
    noise.envelope.loopMode = data.bit(5);
    noise.envelope.useSpeedAsVolume = data.bit(4);
    noise.envelope.speed = data.bits(0,3);
    break;
  }

  case 0x400e: {
    //TODO: Check if the RP2A03E and prior revisions support short mode.
    //Currently assuming that the RP2A03F is bugged. See noise.cpp for bug implementation.
    if(revision != Revision::RP2A03) noise.shortMode = data.bit(7);
    noise.period = data.bits(0,3);
    break;
  }

  case 0x400f: {
    noise.envelope.reloadDecay = true;

    if(enabledChannels.bit(3)) noise.lengthCounter = lengthCounterTable[data.bits(3,7)];
    break;
  }

  case 0x4010: {
    dmc.irqEnable = data.bit(7);
    dmc.loopMode = data.bit(6);
    dmc.period = data.bits(0,3);

    dmc.irqPending = dmc.irqPending && dmc.irqEnable && !dmc.loopMode;
    irqLine();
    break;
  }

  case 0x4011: {
    dmc.dacLatch = data.bits(0,6);
    break;
  }

  case 0x4012: {
    dmc.addrLatch = data;
    break;
  }

  case 0x4013: {
    dmc.lengthLatch = data;
    break;
  }

  case 0x4015: {
    if(!data.bit(0)) pulse[0].lengthCounter = 0;
    if(!data.bit(1)) pulse[1].lengthCounter = 0;
    if(!data.bit(2)) triangle.lengthCounter = 0;
    if(!data.bit(3))    noise.lengthCounter = 0;

    data.bit(4) ? dmc.start() : dmc.stop();
    dmc.irqPending = false;

    irqLine();
    enabledChannels = data.bits(0,4);
    break;
  }

  case 0x4017: {
    frame.mode = data.bits(6,7);

    frame.counter = 0;
    if(frame.mode.bit(1)) clockFrameCounter();
    if(frame.mode.bit(0)) {
      frame.irqPending = false;
      irqLine();
    }
    frame.divider = system.region() == System::Region::NTSC ? FrameCounter::NtscPeriod : FrameCounter::PalPeriod;
    break;
  }

  }
}

auto APU::Filter::runHipassStrong(int sample) -> int {
  hipassStrong += ((((int64)sample << 16) - (hipassStrong >> 16)) * HiPassStrong) >> 16;
  return sample - (hipassStrong >> 32);
}

auto APU::Filter::runHipassWeak(int sample) -> int {
  hipassWeak += ((((int64)sample << 16) - (hipassWeak >> 16)) * HiPassWeak) >> 16;
  return sample - (hipassWeak >> 32);
}

auto APU::Filter::runLopass(int sample) -> int {
  lopass += ((((int64)sample << 16) - (lopass >> 16)) * LoPass) >> 16;
  return (lopass >> 32);
}

auto APU::clockFrameCounter() -> void {
  frame.counter++;

  if(frame.counter & 1) {
    pulse[0].clockLength();
    pulse[0].sweep.clock(0);
    pulse[1].clockLength();
    pulse[1].sweep.clock(1);
    triangle.clockLength();
    noise.clockLength();
  }

  pulse[0].envelope.clock();
  pulse[1].envelope.clock();
  triangle.clockLinearLength();
  noise.envelope.clock();

  if(frame.counter == 0) {
    if(frame.mode & 2) {
      if(system.region() == System::Region::NTSC)
        frame.divider += FrameCounter::NtscPeriod;
      else
        frame.divider += FrameCounter::PalPeriod;
    }
    if(frame.mode == 0) {
      frame.irqPending = true;
      irqLine();
    }
  }
}

auto APU::clockFrameCounterDivider() -> void {
  frame.divider -= 2;
  if(frame.divider <= 0) {
    clockFrameCounter();
    if(system.region() == System::Region::NTSC)
      frame.divider += FrameCounter::NtscPeriod;
    else
      frame.divider += FrameCounter::PalPeriod;
  }
}

const uint8 APU::lengthCounterTable[32] = {
  0x0a, 0xfe, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xa0, 0x08, 0x3c, 0x0a, 0x0e, 0x0c, 0x1a, 0x0e,
  0x0c, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xc0, 0x18, 0x48, 0x1a, 0x10, 0x1c, 0x20, 0x1e,
};

}
