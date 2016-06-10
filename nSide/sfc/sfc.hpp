#pragma once

//based on higan's Super Famicom emulator by byuu,
//with contributions from:
// Andreas Naive     (S-DD1 decompression algorithm)
// anomie
// AWJ               (PPUcounter NTSC/PAL timing)
// Cydrak            (ST-018 discoveries and bug fixes)
// _Demo_
// Derrick Sobodash
// DMV27
// Dr. Decapitator   (DSP-1,2,3,4, ST-010,011,018 ROM dumps)
// FirebrandX
// FitzRoy
// GIGO
// Jonas Quinn       (DSP fixes, Game Boy fixes)
// kode54
// krom
// LostTemplar       (ST-018 program ROM analysis)
// Matthew Callis
// Nach
// neviksti          (SPC7110 decompression algorithm)
// Overload          (Cx4 data ROM dump)
// RedDwarf
// Richard Bannister
// segher            (Cx4 reverse engineering)
// tetsuo55
// TRAC
// zones
// hex_usr           (exportMemory PPU register/expansion chip support, Super Scope turbo cursor)
//license: GPLv3
//original project started: 2004-10-14

#include <emulator/emulator.hpp>
#include <processor/arm/arm.hpp>
#include <processor/gsu/gsu.hpp>
#include <processor/hg51b/hg51b.hpp>
#include <processor/r65816/r65816.hpp>
#include <processor/spc700/spc700.hpp>
#include <processor/upd96050/upd96050.hpp>

namespace SuperFamicom {
  namespace Info {
    static const uint SerializerVersion = 30;
  }
}

#include <libco/libco.h>

#if defined(SFC_SUPERGAMEBOY)
  #include <gb/gb.hpp>
#endif

namespace SuperFamicom {
  struct Thread {
    virtual ~Thread() {
      if(thread) co_delete(thread);
    }

    auto create(auto (*entrypoint)() -> void, uint frequency) -> void {
      if(thread) co_delete(thread);
      thread = co_create(65'536 * sizeof(void*), entrypoint);
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

  //dynamic thread bound to CPU (coprocessors and peripherals)
  struct Cothread : Thread {
    auto step(uint clocks) -> void;
    auto synchronizeCPU() -> void;
  };

  #include <sfc/memory/memory.hpp>
  #include <sfc/ppu/counter/counter.hpp>

  #include <sfc/cpu/cpu.hpp>
  #include <sfc/smp/smp.hpp>
  #include <sfc/dsp/dsp.hpp>
  #include <sfc/ppu/ppu.hpp>

  #include <sfc/controller/controller.hpp>
  #include <sfc/expansion/expansion.hpp>
  #include <sfc/system/system.hpp>
  #include <sfc/scheduler/scheduler.hpp>
  #include <sfc/coprocessor/coprocessor.hpp>
  #include <sfc/slot/slot.hpp>
  #include <sfc/cartridge/cartridge.hpp>
  #include <sfc/cheat/cheat.hpp>

  #include <sfc/memory/memory-inline.hpp>
  #include <sfc/ppu/counter/counter-inline.hpp>

  inline auto Cothread::step(uint clocks) -> void {
    clock += clocks * (uint64)cpu.frequency;
    synchronizeCPU();
  }

  inline auto Cothread::synchronizeCPU() -> void {
    if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
  }
}

#include <sfc/interface/interface.hpp>
