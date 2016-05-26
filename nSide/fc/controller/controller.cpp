#include <fc/fc.hpp>

namespace Famicom {

#include "gamepad/gamepad.cpp"
#include "fourplayers/fourplayers.cpp"
#include "fourscore/fourscore.cpp"
#include "beamgun/beamgun.cpp"
#include "familytrainer/familytrainer.cpp"
#include "vaus/vaus.cpp"
#include "sfcgamepad/sfcgamepad.cpp"
#include "mouse/mouse.cpp"

Controller::Controller(uint port, uint device_id) : port(port), device_id(device_id) {
  if(!thread) create(Controller::Enter, 1);
}

Controller::Controller(uint port) : port(port), device_id(Device::None) {
  if(!thread) create(Controller::Enter, 1);
}

auto Controller::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    if(co_active() == peripherals.controllerPort1->thread) peripherals.controllerPort1->main();
    if(co_active() == peripherals.controllerPort2->thread) peripherals.controllerPort2->main();
    if(co_active() == peripherals.expansionPort->thread)   peripherals.expansionPort->main();
  }
}

auto Controller::main() -> void {
  step(1);
}

auto Controller::poll(uint input) -> int16 {
  return interface->inputPoll(port, interface->deviceRefs[device_id].port[port], input);
}

}
