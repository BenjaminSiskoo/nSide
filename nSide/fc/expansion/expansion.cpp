#include <fc/fc.hpp>

namespace Famicom {

#include "gamepad/gamepad.cpp"
#include "joypair/joypair.cpp"
#include "four-players/four-players.cpp"
#include "beam-gun/beam-gun.cpp"
#include "family-keyboard/family-keyboard.cpp"
#include "family-trainer/family-trainer.cpp"
#include "vaus/vaus.cpp"
#include "sfc-gamepad/sfc-gamepad.cpp"
#include "mouse/mouse.cpp"

Expansion::Expansion() {
  if(!thread) create(Expansion::Enter, 1);
}

Expansion::~Expansion() {
}

auto Expansion::Enter() -> void {
  while(true) scheduler.synchronize(), peripherals.expansionPort->main();
}

auto Expansion::main() -> void {
  step(1);
}

}
