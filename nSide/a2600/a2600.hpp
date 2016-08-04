#pragma once

//Atari 2600 emulator by hex_usr, based on R6502 by byuu
//license: GPLv3
//original project started: 2016-07-31

#include <emulator/emulator.hpp>
#include <emulator/thread.hpp>
#include <emulator/scheduler.hpp>
#include <emulator/cheat.hpp>

#include <processor/r6502/r6502.hpp>

namespace Atari2600 {
  using File = Emulator::File;
  using Scheduler = Emulator::Scheduler;
  using Cheat = Emulator::Cheat;
  extern Scheduler scheduler;
  extern Cheat cheat;

  struct Thread : Emulator::Thread {
    auto create(auto (*entrypoint)() -> void, double frequency) -> void {
      Emulator::Thread::create(entrypoint, frequency);
      scheduler.append(*this);
    }

    inline auto synchronize(Thread& thread) -> void {
      if(clock() >= thread.clock()) scheduler.resume(thread);
    }
  };

  #include <a2600/cpu/cpu.hpp>
  #include <a2600/pia/pia.hpp>
  #include <a2600/tia/tia.hpp>

  #include <a2600/controller/controller.hpp>
  #include <a2600/system/system.hpp>
  #include <a2600/cartridge/cartridge.hpp>
}

#include <a2600/interface/interface.hpp>
