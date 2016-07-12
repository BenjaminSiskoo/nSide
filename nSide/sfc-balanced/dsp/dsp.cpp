#include <sfc-balanced/sfc.hpp>

namespace SuperFamicom {

DSP dsp;

#include "serialization.cpp"
#include "SPC_DSP.cpp"

DSP::DSP() {
}

auto DSP::step(uint clocks) -> void {
  clock += clocks;
}

auto DSP::synchronizeSMP() -> void {
  if(clock >= 0 && !scheduler.synchronizing()) co_switch(smp.thread);
}

auto DSP::main() -> void {
  spc_dsp.run(1);
  step(24);

  int count = spc_dsp.sample_count();
  if(count > 0) {
    for(uint n = 0; n < count; n += 2) stream->sample(samplebuffer[n + 0] / 32768.0, samplebuffer[n + 1] / 32768.0);
    spc_dsp.set_output(samplebuffer, 8192);
  }
}

auto DSP::mute() -> bool {
  return spc_dsp.mute();
}

auto DSP::read(uint8 addr) -> uint8 {
  return spc_dsp.read(addr);
}

auto DSP::write(uint8 addr, uint8 data) -> void {
  spc_dsp.write(addr, data);
}

auto DSP::load(Markup::Node node) -> bool {
  return true;
}

auto DSP::power() -> void {
  spc_dsp.init(smp.apuram);
  spc_dsp.reset();
  spc_dsp.set_output(samplebuffer, 8192);
}

auto DSP::reset() -> void {
  stream = Emulator::audio.createStream(2, 32040.0);
  Thread::clock = 0;
  spc_dsp.soft_reset();
  spc_dsp.set_output(samplebuffer, 8192);
}

}
