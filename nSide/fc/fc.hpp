#pragma once

//based on higan's Famicom emulator by byuu and Ryphecha,
//with contributions from:
// Bisqwit (PPU color generation for NTSC systems)
// FitzRoy (Bug-testing)
// kevtris (PPU voltage levels in Bisqwit's color formula)
// hex_usr (Many code organization changes, PPU timing fixes, mapper implementations, controllers, etc.)
//license: GPLv3
//original project started: 2011-09-05

#include <emulator/emulator.hpp>
#include <emulator/thread.hpp>
#include <emulator/scheduler.hpp>
#include <emulator/cheat.hpp>

#include <processor/r6502/r6502.hpp>
#include <processor/z80/z80.hpp>

namespace Famicom {
  using File = Emulator::File;
  using Thread = Emulator::Thread;
  using Scheduler = Emulator::Scheduler;
  using Cheat = Emulator::Cheat;
  extern Scheduler scheduler;
  extern Cheat cheat;

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
  #include <fc/expansion/expansion.hpp>
  #include <fc/system/system.hpp>
  #include <fc/arcade/arcade.hpp>
  #include <fc/cartridge/cartridge.hpp>

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
