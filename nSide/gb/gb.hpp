#pragma once

#include <emulator/emulator.hpp>
#include <processor/lr35902/lr35902.hpp>

namespace GameBoy {
  namespace Info {
    static const uint SerializerVersion = 4;
  }
}

/*
  based on higan's Game Boy emulator by byuu.
  original author: byuu
  contributors: Jonas Quinn
  license: GPLv3
  original project started: 2010-12-27
*/

#include <libco/libco.h>

namespace GameBoy {
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

  #include <gb/memory/memory.hpp>
  #include <gb/system/system.hpp>
  #include <gb/scheduler/scheduler.hpp>
  #include <gb/cartridge/cartridge.hpp>
  #include <gb/cpu/cpu.hpp>
  #include <gb/ppu/ppu.hpp>
  #include <gb/apu/apu.hpp>
  #include <gb/cheat/cheat.hpp>
}

#include <gb/interface/interface.hpp>
