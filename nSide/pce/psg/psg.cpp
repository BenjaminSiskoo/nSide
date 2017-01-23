#include <pce/pce.hpp>

namespace PCEngine {

PSG psg;

auto PSG::Enter() -> void {
  while(true) scheduler.synchronize(), psg.main();
}

auto PSG::main() -> void {
  step(1);
  stream->sample(0.0, 0.0);
}

auto PSG::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto PSG::power() -> void {
  create(PSG::Enter, 32'087.5);
  stream = Emulator::audio.createStream(2, 32'087.5);
}

}
