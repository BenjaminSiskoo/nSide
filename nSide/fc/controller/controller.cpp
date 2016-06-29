#include <fc/fc.hpp>

namespace Famicom {

#include "gamepad/gamepad.cpp"
#include "gamepad-mic/gamepad-mic.cpp"
#include "fourscore/fourscore.cpp"
#include "zapper/zapper.cpp"
#include "powerpad/powerpad.cpp"
#include "vaus/vaus.cpp"
#include "snesgamepad/snesgamepad.cpp"
#include "mouse/mouse.cpp"

Controller::Controller(bool port) : port(port) {
  if(!thread) create(Controller::Enter, 1);
}

Controller::~Controller() {
}

auto Controller::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    if(co_active() == peripherals.controllerPort1->thread) peripherals.controllerPort1->main();
    if(co_active() == peripherals.controllerPort2->thread) peripherals.controllerPort2->main();
  }
}

auto Controller::main() -> void {
  step(1);
}

}
