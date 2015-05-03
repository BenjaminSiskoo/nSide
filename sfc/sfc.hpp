#ifndef SFC_HPP
#define SFC_HPP

#include <emulator/emulator.hpp>
#include <processor/arm/arm.hpp>
#include <processor/gsu/gsu.hpp>
#include <processor/hg51b/hg51b.hpp>
#include <processor/r65816/r65816.hpp>
#include <processor/spc700/spc700.hpp>
#include <processor/upd96050/upd96050.hpp>

namespace SuperFamicom {
  namespace Info {
    static const char Name[] = "nSide-sfc";
    // Changes from bsnes's serializer version 28:
    // Imported from AWJ's bsnes-classic:
    // * PPUcounter shared serialization for all 3 profiles
    //   (note that this one may or may not be reverted when higan v095 is released)
    static const unsigned SerializerVersion = 0;
  }
}

/*
  nSide-sfc - Super Famicom emulator
  based on higan/bsnes by byuu.
  author: byuu
  contributors:
    blargg  (Balanced/Performance DSP)
    AWJ     (PPUcounter NTSC/PAL timing)
    hex_usr (exportMemory expansion chip support/PPU register dumping)
  license: GPLv3
  original project started: 2004-10-14
*/

#include <libco/libco.h>
#include <gb/gb.hpp>

namespace SuperFamicom {
  struct Thread {
    cothread_t thread;
    unsigned frequency;
    int64 clock;

    inline void create(void (*entrypoint)(), unsigned frequency) {
      if(thread) co_delete(thread);
      thread = co_create(65536 * sizeof(void*), entrypoint);
      this->frequency = frequency;
      clock = 0;
    }

    inline void serialize(serializer& s) {
      s.integer(frequency);
      s.integer(clock);
    }

    inline Thread() : thread(nullptr) {
    }

    inline ~Thread() {
      if(thread) co_delete(thread);
    }
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
  #include <sfc/base/base.hpp>
  #include <sfc/chip/chip.hpp>
  #include <sfc/slot/slot.hpp>
  #include <sfc/cartridge/cartridge.hpp>
  #include <sfc/cheat/cheat.hpp>
  #include <sfc/interface/interface.hpp>

  #include <sfc/memory/memory-inline.hpp>
  #include <sfc/ppu/counter/counter-inline.hpp>
}

#endif
