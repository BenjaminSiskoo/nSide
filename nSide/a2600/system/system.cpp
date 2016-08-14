#include <a2600/a2600.hpp>

namespace Atari2600 {

System system;
Scheduler scheduler;
Cheat cheat;
#include "video.cpp"
#include "peripherals.cpp"
#include "serialization.cpp"

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) tia.refresh();
}

auto System::runToSave() -> void {
  scheduler.synchronize(cpu);
  scheduler.synchronize(pia);
  scheduler.synchronize(tia);
  for(auto peripheral : pia.peripherals) scheduler.synchronize(*peripheral);
}

auto System::init() -> void {
  assert(interface != nullptr);
}

auto System::load() -> bool {
  information = Information();

  if(auto fp = interface->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  auto system = document["system"];

  if(!cartridge.load()) return false;

  switch(cartridge.region()) {
  case Cartridge::Region::NTSC:  information.region = Region::NTSC;  break;
  case Cartridge::Region::PAL:   information.region = Region::PAL;   break;
  case Cartridge::Region::SECAM: information.region = Region::SECAM; break;
  }
  if(system["region"].text() == "NTSC" ) information.region = Region::NTSC;
  if(system["region"].text() == "PAL"  ) information.region = Region::PAL;
  if(system["region"].text() == "SECAM") information.region = Region::SECAM;

  if(!cpu.load(system)) return false;
  if(!pia.load(system)) return false;
  if(!tia.load(system)) return false;

  information.colorburst = region() == Region::NTSC
  ? Emulator::Constants::Colorburst::NTSC
  : Emulator::Constants::Colorburst::PAL * 4.0 / 5.0;

  serializeInit();
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
  configureVideoPalette();
  configureVideoEffects();

  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);

  scheduler.reset();
  cpu.power();
  cpu.reset();
  pia.power();
  tia.power();

  scheduler.primary(cpu);
  peripherals.reset();
}

}
