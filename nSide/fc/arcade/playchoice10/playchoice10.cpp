#include <fc/fc.hpp>

namespace Famicom {

PlayChoice10 playchoice10;

#include "bus.cpp"
#include "cpu.cpp"
#include "video-circuit.cpp"
#include "serialization.cpp"

auto PlayChoice10::init() -> void {
  dip = 0;
  screenConfig = ScreenConfig::Single;
}

auto PlayChoice10::load(Markup::Node node) -> bool {
  if(auto firmware = node["cpu/rom/name"].text()) {
    if(auto fp = interface->open(ID::System, firmware, File::Read, File::Required)) {
      fp->read(pc10bus.bios, 16384);
    } else return false;
  }

  if(auto character = node["pc10/video-circuit/vrom/name"].text()) {
    if(auto fp = interface->open(ID::System, character, File::Read, File::Required)) {
      fp->read(videoCircuit.chrrom, 24576);
    } else return false;
  }

  if(auto palette = node["pc10/video-circuit/cgrom/name"].text()) {
    if(auto fp = interface->open(ID::System, palette, File::Read, File::Required)) {
      fp->read(videoCircuit.cgrom, 768);
    } else return false;
  }

  screenConfig = min(max(node["pc10/screen/mode"].integer(), 1), 2);
  return true;
}

auto PlayChoice10::unload() -> void {
}

auto PlayChoice10::power() -> void {
  pc10bus.power();
  pc10cpu.power();
  videoCircuit.power();
  vramAccess = 1; // 0: Z80,                  1: video circuit
  controls   = 1; // 0: disable START/SELECT, 1: enable START/SELECT
  ppuOutput  = 1; // 0: disable,              1: enable
  apuOutput  = 1; // 0: disable,              1: enable
  cpuReset   = 1; // 0: reset,                1: run
  cpuStop    = 1; // 0: stop,                 1: run
  display    = 1; // 0: video circuit,        1: PPU
  z80NMI     = 0; // 0: disable,              1: enable
  watchdog   = 1; // 0: enable,               1: disable
  ppuReset   = 1; // 0: reset,                1: run
}

auto PlayChoice10::reset() -> void {
  pc10bus.reset();
  pc10cpu.reset();
  videoCircuit.reset();
}

auto PlayChoice10::setDip(uint16 dip) -> void {
  this->dip = dip;
}

}
