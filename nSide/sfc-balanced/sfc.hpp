#pragma once

//clone of higan's Super Famicom emulator's balanced profile.
//Credits for all components excpet for PPU and DSP are available in <sfc/sfc.hpp>.
// blargg            (Balanced/Performance DSP)
// hex_usr           (clean-up of Balanced PPU code)
//license: GPLv3
//maintained since: 2016-04-05

#include <emulator/emulator.hpp>
#include <processor/arm/arm.hpp>
#include <processor/gsu/gsu.hpp>
#include <processor/hg51b/hg51b.hpp>
#include <processor/r65816/r65816.hpp>
#include <processor/spc700/spc700.hpp>
#include <processor/upd96050/upd96050.hpp>

namespace SuperFamicom {
  namespace Info {
    static const uint SerializerVersion = 30 + 0x80000000;
  }
}

#include <libco/libco.h>

#if defined(SFC_SUPERGAMEBOY)
  #include <gb/gb.hpp>
#endif

namespace SuperFamicom {
  struct File {
    static const vfs::file::mode Read = vfs::file::mode::read;
    static const vfs::file::mode Write = vfs::file::mode::write;
    static const bool Required = true;
  };

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

  #include <sfc-balanced/memory/memory.hpp>
  #include <sfc-balanced/ppu/counter/counter.hpp>

  #include <sfc-balanced/cpu/cpu.hpp>
  #include <sfc-balanced/smp/smp.hpp>
  #include <sfc-balanced/dsp/dsp.hpp>
  #include <sfc-balanced/ppu/ppu.hpp>

  #include <sfc-balanced/controller/controller.hpp>
  #include <sfc-balanced/expansion/expansion.hpp>
  #include <sfc-balanced/system/system.hpp>
  #include <sfc-balanced/scheduler/scheduler.hpp>
  #include <sfc-balanced/coprocessor/coprocessor.hpp>
  #include <sfc-balanced/slot/slot.hpp>
  #include <sfc-balanced/cartridge/cartridge.hpp>
  #include <sfc-balanced/cheat/cheat.hpp>

  #include <sfc-balanced/memory/memory-inline.hpp>
  #include <sfc-balanced/ppu/counter/counter-inline.hpp>

  inline auto Cothread::step(uint clocks) -> void {
    clock += clocks * (uint64)cpu.frequency;
    synchronizeCPU();
  }

  inline auto Cothread::synchronizeCPU() -> void {
    if(clock >= 0 && !scheduler.synchronizing()) co_switch(cpu.thread);
  }
}

#include <sfc-balanced/interface/interface.hpp>
