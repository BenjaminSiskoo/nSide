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
    // * CPU/PPU timing refactoring to make History management simpler
    // * PPUcounter shared serialization for all 3 profiles
    //   (note that these ones may or may not be reverted when higan v095 is released)
    static const unsigned SerializerVersion = 1;
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
