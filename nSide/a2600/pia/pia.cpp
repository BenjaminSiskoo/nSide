#include <a2600/a2600.hpp>

namespace Atari2600 {

PIA pia;

#include "memory.cpp"
#include "io.cpp"
#include "serialization.cpp"

PIA::PIA() {
}

PIA::~PIA() {
}

auto PIA::step(uint clocks) -> void {
  //io.timer = (io.timer - 1) & io.timerMask;
  Thread::step(clocks);
  for(auto peripheral : peripherals) synchronize(*peripheral);
  synchronize(cpu);
}

auto PIA::Enter() -> void {
  while(true) scheduler.synchronize(), pia.main();
}

auto PIA::main() -> void {
  if(interface->inputPoll(ID::Port::Hardware, ID::Device::Controls, 2)) {
    if(!input.color) io.color ^= 1;
    input.color = 1;
  } else {
    input.color = 0;
  }

  if(interface->inputPoll(ID::Port::Hardware, ID::Device::Controls, 3)) {
    if(!input.difficulty0) io.difficulty0 ^= 1;
    input.difficulty0 = 1;
  } else {
    input.difficulty0 = 0;
  }

  if(interface->inputPoll(ID::Port::Hardware, ID::Device::Controls, 4)) {
    if(!input.difficulty1) io.difficulty1 ^= 1;
    input.difficulty1 = 1;
  } else {
    input.difficulty1 = 0;
  }
  step(1);
}

auto PIA::load(Markup::Node node) -> bool {
  return true;
}

auto PIA::power() -> void {
  create(Enter, system.colorburst() / 3.0);

  io.color = 1;
  io.difficulty0 = 0;
  io.difficulty1 = 0;

  io.swcha  = 0x00;
  io.swacnt = 0x00;
  io.swchb  = 0x00;
  io.swbcnt = 0x00;

  io.timer     = 0x00000;
  io.timerMask = 0x000ff;

  io.timerInterrupt = false;
  io.pa7Interrupt = false;
  io.edgeDetect = false;
}

}
