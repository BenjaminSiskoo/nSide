#include <ms/ms.hpp>

namespace MasterSystem {

VDP vdp;
#include "io.cpp"
#include "render.cpp"

auto VDP::Enter() -> void {
  while(true) scheduler.synchronize(), vdp.main();
}

auto VDP::main() -> void {
  for(uint y : range(262)) {
    for(uint x : range(342)) {
      run();
      step(1);
    }
    if(y == activeHeight()) scheduler.exit(Scheduler::Event::Frame);
  }
}

auto VDP::step(uint clocks) -> void {
  if(++io.hcounter == 684) {
    io.hcounter = 0;
    if(++io.vcounter == 262) {
      io.vcounter = 0;
    }
  }

  Thread::step(clocks);
  synchronize(cpu);
}

auto VDP::refresh() -> void {
  Emulator::video.refresh(buffer, 256 * sizeof(uint32), screenWidth(), screenHeight());
}

auto VDP::power() -> void {
}

auto VDP::reset() -> void {
  create(VDP::Enter, system.colorburst() * 15.0 / 5.0);

  memory::fill(&io, sizeof(IO));
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
  return system.model() != Model::GameGear ? 240 : 144;
}

inline auto VDP::activeWidth() -> uint {
  return 256;
}

inline auto VDP::activeHeight() -> uint {
  return system.model() == Model::SG1000 ? 192 : 240;
}

}
