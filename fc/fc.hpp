#ifndef FC_HPP
#define FC_HPP

#include <emulator/emulator.hpp>
#include <processor/r6502/r6502.hpp>

namespace Famicom {
  namespace Info {
    static const string Name[] = "nSide-fc";
    // If you import nSide's changes into another fork of higan, please do not
    // import the serializer version ID unless you intend to import all of
    // nSide-fc simultaneously.
    // This is required to avoid errors in case a save state created in one
    // higan fork is loaded into another.
    static const unsigned SerializerVersion = 0;
  }
}

/*
  nSide-fc - Famicom emulator
  based on higan/bnes by byuu and Ryphecha.
  authors: byuu, Ryphecha
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

    auto create(void (*entrypoint)(), unsigned frequency) -> void {
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
    unsigned frequency = 0;
    int64_t clock = 0;
  };

  #include <fc/memory/memory.hpp>
  #include <fc/ppu/counter/counter.hpp>

  #include <fc/cpu/cpu.hpp>
  #include <fc/apu/apu.hpp>
  #include <fc/ppu/ppu.hpp>

  #include <fc/controller/controller.hpp>
  #include <fc/system/system.hpp>
  #include <fc/arcade/arcade.hpp>
  #include <fc/cartridge/cartridge.hpp>
  #include <fc/cheat/cheat.hpp>
  #include <fc/interface/interface.hpp>

  #include <fc/memory/memory-inline.hpp>
  #include <fc/ppu/counter/counter-inline.hpp>
}

#endif
