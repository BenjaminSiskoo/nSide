#include <fc/fc.hpp>

namespace Famicom {

PlayChoice10 playchoice10;

#include "bus.cpp"
#include "cpu.cpp"
#include "video-circuit.cpp"
#include "serialization.cpp"

auto PlayChoice10::init() -> void {
}

auto PlayChoice10::load(Markup::Node node) -> bool {
  if(auto firmware = node["pc10/cpu/rom/name"].text()) {
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

  setDip(interface->dipSettings(node["pc10"]));
  //dip.bits( 0, 5) = 0;  //Price
  //dip.bit (    6) = 1;  //Enable Sound during Attract Mode
  //dip.bit (    7) = 0;  //Self-test on Power-up
  //dip.bits( 8,13) = 0;  //Timer Speed
  //dip.bit (   14) = 0;  //Divide price by 2
  //dip.bit (   15) = 0;  //Free play (if bits 8..14 are 0)

  return true;
}

auto PlayChoice10::unload() -> void {
}

auto PlayChoice10::power() -> void {
  pc10bus.power();
  pc10cpu.power();
  videoCircuit.power();

  nmiDetected = false;

  vramAccess = 0;  //0: Z80,                  1: video circuit
  controls   = 0;  //0: disable START/SELECT, 1: enable START/SELECT
  ppuOutput  = 0;  //0: disable,              1: enable
  apuOutput  = 0;  //0: disable,              1: enable
  cpuReset   = 0;  //0: reset,                1: run
  cpuStop    = 0;  //0: stop,                 1: run
  display    = 0;  //0: video circuit,        1: PPU
  z80NMI     = 0;  //0: disable,              1: enable
  watchdog   = 0;  //0: enable,               1: disable
  ppuReset   = 0;  //0: reset,                1: run
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
