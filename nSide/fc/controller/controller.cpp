#include <fc/fc.hpp>

namespace Famicom {

#include "gamepad/gamepad.cpp"
#include "fourplayers/fourplayers.cpp"
#include "fourscore/fourscore.cpp"
#include "beamgun/beamgun.cpp"
#include "familytrainer/familytrainer.cpp"
#include "sfcgamepad/sfcgamepad.cpp"
#include "mouse/mouse.cpp"
#include "vspanel/vspanel.cpp"

Controller::Controller(uint port, uint device_id) : port(port), device_id(device_id) {
  if(!thread) create(Controller::Enter, 1);
}

Controller::Controller(uint port) : port(port), device_id((uint)Device::ID::None) {
  if(!thread) create(Controller::Enter, 1);
}

auto Controller::Enter() -> void {
  if(co_active() == device.controllerPort1->thread) device.controllerPort1->enter();
  if(co_active() == device.controllerPort2->thread) device.controllerPort2->enter();
  if(co_active() == device.expansionPort->thread) device.expansionPort->enter();
}

auto Controller::enter() -> void {
  while(true) step(1);
}

auto Controller::step(uint clocks) -> void {
  clock += clocks * (uint64)cpu.frequency;
  synchronizeCPU();
}

auto Controller::synchronizeCPU() -> void {
  if(clock >= 0 && scheduler.sync != Scheduler::SynchronizeMode::All) co_switch(cpu.thread);
}

auto Controller::poll(uint input) -> int16 {
  return interface->inputPoll(port, interface->device_ref[device_id].port[port], input);
}

}
