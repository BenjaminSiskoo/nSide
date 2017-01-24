#include <fc/fc.hpp>

namespace Famicom {

System system;
Scheduler scheduler;
Cheat cheat;
#include "video.cpp"
#include "peripherals.cpp"
#include "random.cpp"
#include "serialization.cpp"

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) {
    if(model() == Model::FamicomBox) famicombox.pollInputs();
    if(model() != Model::VSSystem || vssystem.gameCount == 2) ppu0.refresh();
    if(model() == Model::VSSystem) ppu1.refresh();
    if(model() == Model::PlayChoice10) playchoice10.videoCircuit.refresh();
  }
}

auto System::runToSave() -> void {
  if(model() != Model::VSSystem || vssystem.gameCount == 2) {
    scheduler.synchronize(cpu0);
    scheduler.synchronize(apu0);
    scheduler.synchronize(ppu0);
  }

  if(model() == Model::VSSystem) {
    scheduler.synchronize(cpu1);
    scheduler.synchronize(apu1);
    scheduler.synchronize(ppu1);
  }

  scheduler.synchronize(cartridge);

  if(model() != Model::VSSystem) {
    for(auto coprocessor : cpu0.coprocessors) scheduler.synchronize(*coprocessor);
    for(auto peripheral : cpu0.peripherals) scheduler.synchronize(*peripheral);
  }

  if(model() == Model::VSSystem) {
    for(auto coprocessor : cpu1.coprocessors) scheduler.synchronize(*coprocessor);
    for(auto peripheral : cpu1.peripherals) scheduler.synchronize(*peripheral);
  }
}

auto System::load(Emulator::Interface* interface, Model model) -> bool {
  information = {};
  information.model = model;

  if(auto fp = platform->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  auto system = document["system"];

  bus0.reset();
  bus1.reset();
  if(!cartridge.load()) return false;

  switch(cartridge.region()) {
  case Cartridge::Region::NTSC:  information.region = Region::NTSC;  break;
  case Cartridge::Region::PAL:   information.region = Region::PAL;   break;
  case Cartridge::Region::Dendy: information.region = Region::Dendy; break;
  }
  if(system["region"].text() == "NTSC" ) information.region = Region::NTSC;
  if(system["region"].text() == "PAL"  ) information.region = Region::PAL;
  if(system["region"].text() == "Dendy") information.region = Region::Dendy;

  if(!cpu0.load(system)) return false;
  if(!apu0.load(system)) return false;
  if(!ppu0.load(system)) return false;

  if(model == Model::VSSystem) {
    if(!cpu1.load(system)) return false;
    if(!apu1.load(system)) return false;
    if(!ppu1.load(system)) return false;
  }

  information.colorburst = region() == Region::NTSC
  ? Emulator::Constants::Colorburst::NTSC
  : Emulator::Constants::Colorburst::PAL;

  switch(model) {

  case Model::VSSystem: {
    vssystem.load();
    break;
  }

  case Model::PlayChoice10: {
    if(!playchoice10.load(system)) return false;
    break;
  }

  case Model::FamicomBox: {
    if(!famicombox.load(system)) return false;
    break;
  }

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
  //Emulator::video.resize() is called in configureVideoEffects()
  configureVideoPalette();
  configureVideoEffects();

  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);

  scheduler.reset();
  cartridge.power();

  if(model() != Model::VSSystem || vssystem.gameCount == 2) {
    cpu0.power();
    apu0.power();
    ppu0.power();
  }

  if(model() == Model::VSSystem) {
    cpu1.power();
    apu1.power();
    ppu1.power();
  }

  switch(model()) {
  case Model::VSSystem:     vssystem.power(); break;
  case Model::PlayChoice10: playchoice10.power(); break;
  case Model::FamicomBox:   famicombox.power(); break;
  }

  switch(model()) {
  case Model::VSSystem:     cpu0.coprocessors.append(&vssystem); cpu1.coprocessors.append(&vssystem); break;
  case Model::PlayChoice10: cpu0.coprocessors.append(&playchoice10.pc10cpu); break;
  case Model::FamicomBox:   cpu0.coprocessors.append(&famicombox); break;
  }

  scheduler.primary(model() == Model::VSSystem && vssystem.gameCount == 1 ? cpu1 : cpu0);
  peripherals.reset();
}

}
