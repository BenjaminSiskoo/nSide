#include <ms/ms.hpp>

namespace MasterSystem {

VDP vdp;
#include "io.cpp"
#include "render.cpp"

auto VDP::Enter() -> void {
  while(true) scheduler.synchronize(), vdp.main();
}

auto VDP::main() -> void {
  scanline();
  if(state.y < activeHeight()) {
    for(uint x : range(activeWidth())) {
      run();
      step(4);
    }
    step(344);
    if(state.y == screenY() + screenHeight()) scheduler.exit(Scheduler::Event::Frame);
  } else {
    step(1368);
  }
}

auto VDP::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto VDP::refresh() -> void {
  Emulator::video.refresh(buffer, 256 * sizeof(uint32), screenWidth(), screenHeight());
}

auto VDP::power() -> void {
}

auto VDP::reset() -> void {
  create(VDP::Enter, system.colorburst() * 6.0);
}

auto VDP::vblank() -> bool {
  return false;
}

inline auto VDP::screenX() -> uint {
  return system.model() != Model::GameGear ? 0 : (256 - 160) / 2;
}

inline auto VDP::screenY() -> uint {
  return system.model() != Model::GameGear ? 0 : (240 - 144) / 2;
}

inline auto VDP::screenWidth() -> uint {
  return system.model() != Model::GameGear ? 256 : 160;
}

inline auto VDP::screenHeight() -> uint {
  switch(system.model()) {
  case Model::SG1000: return 192;
  case Model::MasterSystem: return 240;
  case Model::GameGear: return 144;
  }
}

inline auto VDP::activeWidth() -> uint {
  return 256;
}

inline auto VDP::activeHeight() -> uint {
  return system.model() == Model::SG1000 ? 192 : 240;
}

}
