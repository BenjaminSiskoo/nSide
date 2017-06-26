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
    Emulator::video.refreshFinal();
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

  if(model() != Model::VSSystem || vssystem.gameCount == 2) {
    scheduler.synchronize(cartridgeSlot[bus0.slot]);
    for(auto coprocessor : cpu0.coprocessors) scheduler.synchronize(*coprocessor);
    for(auto peripheral : cpu0.peripherals) scheduler.synchronize(*peripheral);
  }

  if(model() == Model::VSSystem) {
    scheduler.synchronize(cartridgeSlot[bus1.slot]);
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

  bus0.slot = 0;
  bus0.reset();
  bus1.slot = 1;
  bus1.reset();
  cartridgeSlot.append(Cartridge(0));
  if(model == Model::VSSystem) cartridgeSlot.append(Cartridge(1));
  if(!cartridgeSlot[model == Model::VSSystem].load()) return false;

  if(model == Model::PlayChoice10) for(uint slot : range(1, 10)) {
    auto cartridge = Cartridge(slot);
    if(!cartridge.load()) break;
    cartridgeSlot.append(cartridge);
  }
  if(model == Model::FamicomBox) for(uint slot : range(1, 15)) {
    auto cartridge = Cartridge(slot);
    if(!cartridge.load()) break;
    cartridgeSlot.append(cartridge);
  }

  if(model == Model::Famicom) {
    if(cartridgeSlot[0].region() == "NTSC-J") {
      information.region = Region::NTSCJ;
      information.frequency = Emulator::Constants::Colorburst::NTSC * 6.0;
    }
    if(cartridgeSlot[0].region() == "NTSC-U") {
      information.region = Region::NTSCU;
      information.frequency = Emulator::Constants::Colorburst::NTSC * 6.0;
    }
    if(cartridgeSlot[0].region() == "PAL") {
      information.region = Region::PAL;
      information.frequency = Emulator::Constants::Colorburst::PAL * 6.0;
    }
    if(cartridgeSlot[0].region() == "Dendy") {
      information.region = Region::Dendy;
      information.frequency = Emulator::Constants::Colorburst::PAL * 6.0;
    }
  } else {
    information.region = model == Model::PlayChoice10 ? Region::NTSCU : Region::NTSCJ;
    information.frequency = Emulator::Constants::Colorburst::NTSC * 6.0;
  }

  if(!cpu0.load(system)) return false;
  if(!apu0.load(system)) return false;
  if(!ppu0.load(system)) return false;

  if(model == Model::VSSystem) {
    if(!cpu1.load(system)) return false;
    if(!apu1.load(system)) return false;
    if(!ppu1.load(system)) return false;
  }

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
  if(model() != Model::VSSystem) {
    for(auto& cartridge : cartridgeSlot) cartridge.save();
  } else {
    cartridgeSlot[2 - vssystem.gameCount].save();
  }
}

auto System::unload() -> void {
  if(!loaded()) return;
  peripherals.unload();

  if(model() != Model::VSSystem) {
    for(auto& cartridge : cartridgeSlot) cartridge.unload();
  } else {
    cartridgeSlot[2 - vssystem.gameCount].unload();
  }

  if(model() == Model::FamicomBox) famicombox.unload();

  cartridgeSlot.reset();
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

  if(model() != Model::VSSystem || vssystem.gameCount == 2) {
    cartridgeSlot[bus0.slot].power();
    cpu0.power();
    apu0.power();
    ppu0.power();
  }

  if(model() == Model::VSSystem) {
    cartridgeSlot[bus1.slot].power();
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
