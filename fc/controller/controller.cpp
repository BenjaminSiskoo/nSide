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

auto Controller::Enter() -> void {
  if(co_active() == input.port1->thread) input.port1->enter();
  if(co_active() == input.port2->thread) input.port2->enter();
  if(co_active() == input.expansion->thread) input.expansion->enter();
}

auto Controller::enter() -> void {
  while(true) step(1);
}

auto Controller::step(unsigned clocks) -> void {
  clock += clocks * (uint64)cpu.frequency;
  synchronize_cpu();
}

auto Controller::synchronize_cpu() -> void {
  if(clock >= 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(cpu.thread);
}

auto Controller::poll(unsigned input) -> int16_t {
  return interface->inputPoll(port, interface->device_ref[device].port[port], input);
}

}
