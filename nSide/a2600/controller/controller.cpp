#include <a2600/a2600.hpp>

namespace Atari2600 {

#include "joystick/joystick.cpp"

Controller::Controller(uint port) : port(port) {
  if(!handle()) create(Controller::Enter, 1);
}

Controller::~Controller() {
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
  synchronize(pia);
}

}
