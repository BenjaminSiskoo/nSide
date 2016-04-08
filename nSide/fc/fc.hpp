#pragma once

#include <emulator/emulator.hpp>
#include <processor/r6502/r6502.hpp>

namespace Famicom {
  namespace Info {
    // If you import nSide's changes into another fork of higan, please do not
    // import the serializer version ID unless you intend to import all of
    // nSide-fc simultaneously.
    // This is required to avoid errors in case a save state created in one
    // higan fork is loaded into another.
    static const uint SerializerVersion = 0;
  }
}

/*
  based on higan's Famicom emulator.
  original authors: byuu, Ryphecha
  contributors: FitzRoy, hex_usr
  license: GPLv3
  original project started: 2011-09-05
*/

#include <libco/libco.h>

namespace Famicom {
  struct Thread {
    ~Thread() {
      if(thread) co_delete(thread);
    }

    auto create(auto (*entrypoint)() -> void, uint frequency) -> void {
      if(thread) co_delete(thread);
      thread = co_create(65536 * sizeof(void*), entrypoint);
      this->frequency = frequency;
      clock = 0;
    }

    auto serialize(serializer& s) -> void {
      s.integer(frequency);
      s.integer(clock);
    }

    cothread_t thread = nullptr;
    uint frequency = 0;
    int64 clock = 0;
  };

  //dynamic thread bound to CPU (arcade processors and timers)
  struct Cothread : Thread {
    auto step(uint clocks) -> void;
    auto synchronizeCPU() -> void;
  };

  #include <fc/memory/memory.hpp>
  #include <fc/ppu/counter/counter.hpp>

  #include <fc/cpu/cpu.hpp>
  #include <fc/apu/apu.hpp>
  #include <fc/ppu/ppu.hpp>

  #include <fc/controller/controller.hpp>
  #include <fc/system/system.hpp>
  #include <fc/scheduler/scheduler.hpp>
  #include <fc/arcade/arcade.hpp>
  #include <fc/cartridge/cartridge.hpp>
  #include <fc/cheat/cheat.hpp>

  #include <fc/memory/memory-inline.hpp>
  #include <fc/ppu/counter/counter-inline.hpp>

  inline auto Cothread::step(uint clocks) -> void {
    clock += clocks * (uint64)cpu.frequency;
    synchronizeCPU();
  }

  inline auto Cothread::synchronizeCPU() -> void {
    if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
  }
}

#include <fc/interface/interface.hpp>
