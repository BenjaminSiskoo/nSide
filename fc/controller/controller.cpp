#include <fc/fc.hpp>

#define CONTROLLER_CPP
namespace Famicom {

#include "gamepad/gamepad.cpp"
#include "fourplayers/fourplayers.cpp"
#include "fourscore/fourscore.cpp"
#include "beamgun/beamgun.cpp"
#include "familytrainer/familytrainer.cpp"
#include "sfcgamepad/sfcgamepad.cpp"
#include "mouse/mouse.cpp"
#include "vspanel/vspanel.cpp"

void Controller::Enter() {
  if(co_active() == input.port1->thread) input.port1->enter();
  if(co_active() == input.port2->thread) input.port2->enter();
  if(co_active() == input.expansion->thread) input.expansion->enter();
}

void Controller::enter() {
  while(true) step(1);
}

void Controller::step(unsigned clocks) {
  clock += clocks * (uint64)cpu.frequency;
  synchronize_cpu();
}

void Controller::synchronize_cpu() {
  if(clock >= 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(cpu.thread);
}

Controller::Controller(unsigned port, unsigned device):
port(port),
device(device) {
  if(!thread) create(Controller::Enter, 1);
}

Controller::Controller(unsigned port):
port(port),
device((unsigned)Input::Device::None) {
  if(!thread) create(Controller::Enter, 1);
}

int16_t Controller::poll(unsigned input) {
  return interface->inputPoll(port, interface->device_ref[device].port[port], input);
}

}
