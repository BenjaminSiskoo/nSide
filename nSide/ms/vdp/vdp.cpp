#include <ms/ms.hpp>

namespace MasterSystem {

VDP vdp;

#include "io.cpp"

auto VDP::Enter() -> void {
  while(true) scheduler.synchronize(), vdp.main();
}

auto VDP::main() -> void {
  step(system.colorburst() * 6.0);
}

auto VDP::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
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
