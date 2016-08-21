#include <md/md.hpp>

namespace MegaDrive {

#include "control-pad/control-pad.cpp"
#include "fighting-pad-6b/fighting-pad-6b.cpp"

Controller::Controller(uint port) : port(port) {
  if(!handle()) create(Controller::Enter, 100);
}

Controller::~Controller() {
  scheduler.remove(*this);
}

auto Controller::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    if(peripherals.controllerPort1->active()) peripherals.controllerPort1->main();
    if(peripherals.controllerPort2->active()) peripherals.controllerPort2->main();
    if(peripherals.extensionPort->active())   peripherals.extensionPort->main();
  }
}

auto Controller::main() -> void {
  step(1);
  synchronize(cpu);
}

}
