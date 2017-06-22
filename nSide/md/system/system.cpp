#include <md/md.hpp>

namespace MegaDrive {

System system;
Scheduler scheduler;
Cheat cheat;
#include "peripherals.cpp"
#include "serialization.cpp"

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) vdp.refresh();
}

auto System::runToSave() -> void {
  scheduler.synchronize(cpu);
  scheduler.synchronize(apu);
  scheduler.synchronize(vdp);
  scheduler.synchronize(psg);
  scheduler.synchronize(ym2612);
}

auto System::load(Emulator::Interface* interface, maybe<Region> region) -> bool {
  information = {};

  if(auto fp = platform->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  auto system = document["system"];

  if(!busCPU.load(system)) return false;
  if(!cpu.load(system)) return false;
  if(!apu.load(system)) return false;
  if(!vdp.load(system)) return false;
  if(!psg.load(system)) return false;
  if(!ym2612.load(system)) return false;
  if(!cartridge.load()) return false;

  if(cartridge.region() == "NTSC-J") {
    information.region = Region::NTSCJ;
    information.colorburst = Emulator::Constants::Colorburst::NTSC;
  }
  if(cartridge.region() == "NTSC-U") {
    information.region = Region::NTSCU;
    information.colorburst = Emulator::Constants::Colorburst::NTSC;
  }
  if(cartridge.region() == "PAL") {
    information.region = Region::PAL;
    information.colorburst = Emulator::Constants::Colorburst::PAL * 4.0 / 5.0;
  }

  serializeInit();
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
  Emulator::video.reset();
  Emulator::video.setInterface(interface);
  Emulator::video.setPalette();

  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);

  scheduler.reset();
  cartridge.power();
  cpu.power();
  apu.power();
  vdp.power();
  psg.power();
  ym2612.power();
  scheduler.primary(cpu);

  peripherals.reset();
}

}
