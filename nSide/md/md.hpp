#pragma once

//based on higan's Mega Drive emulator by byuu
//license: GPLv3
//original project started: 2016-07-08

#include <emulator/emulator.hpp>
#include <emulator/thread.hpp>
#include <emulator/scheduler.hpp>

#include <processor/m68k/m68k.hpp>
#include <processor/z80/z80.hpp>

namespace MegaDrive {
  using File = Emulator::File;
  using Scheduler = Emulator::Scheduler;
  extern Scheduler scheduler;

  struct Thread : Emulator::Thread {
    auto create(auto (*entrypoint)() -> void, double frequency) -> void {
      Emulator::Thread::create(entrypoint, frequency);
      scheduler.append(*this);
    }

    inline auto synchronize(Thread& thread) -> void {
      if(clock() >= thread.clock()) scheduler.resume(thread);
    }
  };

  #include <md/cpu/cpu.hpp>
  #include <md/apu/apu.hpp>
  #include <md/vdp/vdp.hpp>
  #include <md/psg/psg.hpp>
  #include <md/ym2612/ym2612.hpp>

  #include <md/controller/controller.hpp>
  #include <md/system/system.hpp>
  #include <md/cartridge/cartridge.hpp>
}

#include <md/interface/interface.hpp>
