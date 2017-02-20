#include <ms/ms.hpp>

namespace MasterSystem {

PSG psg;
#include "io.cpp"

auto PSG::Enter() -> void {
  while(true) scheduler.synchronize(), psg.main();
}

auto PSG::main() -> void {
  if(system.model() != Model::GameGear) {
    stream->sample(0.0);
  } else {
    stream->sample(0.0, 0.0);
  }
  step(1);
}

auto PSG::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto PSG::power() -> void {
  create(PSG::Enter, system.colorburst());
  if(system.model() != Model::GameGear) {
    stream = Emulator::audio.createStream(1, system.colorburst());
  } else {
    stream = Emulator::audio.createStream(2, system.colorburst());
  }
}

}
