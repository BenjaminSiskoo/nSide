#pragma once

#include <emulator/emulator.hpp>
#include <processor/arm/arm.hpp>
#include <processor/gsu/gsu.hpp>
#include <processor/hg51b/hg51b.hpp>
#include <processor/r65816/r65816.hpp>
#include <processor/spc700/spc700.hpp>
#include <processor/upd96050/upd96050.hpp>

namespace SuperFamicom {
  namespace Info {
    static const string Name = "nSide-sfc";
    static const uint SerializerVersion = 29;
  }
}

/*
  nSide-sfc - Super Famicom emulator
  based on higan/bsnes by byuu.
  author: byuu
  contributors:
    Andreas Naive     (S-DD1 decompression algorithm)
    anomie
    AWJ               (PPUcounter NTSC/PAL timing)
    blargg            (Balanced/Performance DSP)
    Cydrak            (ST-018 discoveries and bug fixes)
    _Demo_
    Derrick Sobodash
    DMV27
    Dr. Decapitator   (DSP-1,2,3,4, ST-010,011,018 ROM dumps)
    FirebrandX
    FitzRoy
    GIGO
    Jonas Quinn       (DSP fixes, Game Boy fixes)
    kode54
    krom
    LostTemplar       (ST-018 program ROM analysis)
    Matthew Callis
    Nach
    neviksti          (SPC7110 decompression algorithm)
    Overload          (Cx4 data ROM dump)
    RedDwarf
    Richard Bannister
    segher            (Cx4 reverse engineering)
    tetsuo55
    TRAC
    zones
    hex_usr           (exportMemory PPU register/expansion chip support)
  license: GPLv3
  original project started: 2004-10-14
*/

#include <libco/libco.h>
#include <gb/gb.hpp>

#if defined(PROFILE_PERFORMANCE)
  #include <nall/priority-queue.hpp>
#endif

namespace SuperFamicom {
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

  #include <sfc/memory/memory.hpp>
  #include <sfc/ppu/counter/counter.hpp>

  #if defined(PROFILE_ACCURACY)
  #include "profile-accuracy.hpp"
  #elif defined(PROFILE_BALANCED)
  #include "profile-balanced.hpp"
  #elif defined(PROFILE_PERFORMANCE)
  #include "profile-performance.hpp"
  #endif

  #include <sfc/controller/controller.hpp>
  #include <sfc/system/system.hpp>
  #include <sfc/expansion/expansion.hpp>
  #include <sfc/coprocessor/coprocessor.hpp>
  #include <sfc/slot/slot.hpp>
  #include <sfc/cartridge/cartridge.hpp>
  #include <sfc/cheat/cheat.hpp>

  #include <sfc/memory/memory-inline.hpp>
  #include <sfc/ppu/counter/counter-inline.hpp>
}

#include <sfc/interface/interface.hpp>
