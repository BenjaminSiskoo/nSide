#include <fc/fc.hpp>

namespace Famicom {

#define cpu (Model::VSSystem() ? cpu1 : cpu0)
#define ppu (Model::VSSystem() ? ppu1 : ppu0)

#include "gamepad/gamepad.cpp"
#include "gamepad-mic/gamepad-mic.cpp"
#include "four-score/four-score.cpp"
#include "zapper/zapper.cpp"
#include "power-pad/power-pad.cpp"
#include "vaus/vaus.cpp"
#include "snes-gamepad/snes-gamepad.cpp"
#include "mouse/mouse.cpp"

Controller::Controller(uint port) : port(port) {
  if(!handle()) create(Controller::Enter, 1);
}

Controller::~Controller() {
  scheduler.remove(*this);
}

auto Controller::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    if(peripherals.controllerPort1->active()) peripherals.controllerPort1->main();
    if(peripherals.controllerPort2->active()) peripherals.controllerPort2->main();
  }
}

auto Controller::main() -> void {
  step(1);
  synchronize(cpu);
}

#undef cpu
#undef ppu

}
