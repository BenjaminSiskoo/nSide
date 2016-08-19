#include <ms/ms.hpp>

namespace MasterSystem {

VDP vdp;

#include "io.cpp"

auto VDP::Enter() -> void {
  while(true) scheduler.synchronize(), vdp.main();
}

auto VDP::main() -> void {
  for(uint y : range(262)) {
    for(uint x : range(342)) {
      step(4);
    }
    if(y == 240) scheduler.exit(Scheduler::Event::Frame);
  }
}

auto VDP::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto VDP::refresh() -> void {
  uint width = system.model() != Model::GameGear ? 256 : 160;
  uint height = system.model() == Model::SG1000 ? 192 : system.model() == Model::MasterSystem ? 240 : 144;
  Emulator::video.refresh(buffer, 256 * sizeof(uint32), width, height);
}

auto VDP::power() -> void {
}

auto VDP::reset() -> void {
  create(VDP::Enter, system.colorburst() * 6.0);
}

auto VDP::vblank() -> bool {
  return false;
}

}
