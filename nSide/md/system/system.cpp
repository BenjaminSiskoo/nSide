#include <md/md.hpp>

namespace MegaDrive {

#include "peripherals.cpp"
System system;
Scheduler scheduler;

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) {
    if(platform->inputPoll(ID::Port::Hardware, ID::Device::Controls, 0)) reset();
    vdp.refresh();
  }
}

auto System::load(Emulator::Interface* interface) -> bool {
  information = {};

  if(auto fp = platform->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  if(!cartridge.load()) return false;

  information.colorburst = Emulator::Constants::Colorburst::NTSC;
  this->interface = interface;
  return information.loaded = true;
}

auto System::save() -> void {
  if(!loaded()) return;
  cartridge.save();
}

auto System::unload() -> void {
  if(!loaded()) return;
  peripherals.unload();

  cartridge.unload();
  information.loaded = false;
}

auto System::power() -> void {
  cartridge.power();
  cpu.power();
  apu.power();
  vdp.power();
  psg.power();
  ym2612.power();
  reset();
}

auto System::reset() -> void {
  Emulator::video.reset();
  Emulator::video.setInterface(interface);
  Emulator::video.setPalette();

  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);

  scheduler.reset();
  cartridge.reset();
  cpu.reset();
  apu.reset();
  vdp.reset();
  psg.reset();
  ym2612.reset();
  scheduler.primary(cpu);

  peripherals.reset();
}

}
