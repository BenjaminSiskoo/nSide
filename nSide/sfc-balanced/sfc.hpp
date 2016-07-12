#pragma once

//clone of higan's Super Famicom emulator's balanced profile.
//Credits for all components excpet for PPU and DSP are available in <sfc/sfc.hpp>.
// blargg            (Balanced/Performance DSP)
// hex_usr           (clean-up of Balanced PPU code)
//license: GPLv3
//original project started: 2004-10-14

#include <emulator/emulator.hpp>
#include <emulator/thread.hpp>
#include <emulator/scheduler.hpp>
#include <emulator/cheat.hpp>

#include <processor/arm/arm.hpp>
#include <processor/gsu/gsu.hpp>
#include <processor/hg51b/hg51b.hpp>
#include <processor/r65816/r65816.hpp>
#include <processor/spc700/spc700.hpp>
#include <processor/upd96050/upd96050.hpp>

#if defined(SFC_SUPERGAMEBOY)
  #include <gb/gb.hpp>
#endif

namespace SuperFamicom {
  using File = Emulator::File;
  using Thread = Emulator::Thread;
  using Scheduler = Emulator::Scheduler;
  using Cheat = Emulator::Cheat;
  extern Scheduler scheduler;
  extern Cheat cheat;

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
  #include <sfc-balanced/coprocessor/coprocessor.hpp>
  #include <sfc-balanced/slot/slot.hpp>
  #include <sfc-balanced/cartridge/cartridge.hpp>

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
