#include <fc/fc.hpp>

namespace Famicom {

System system;
Scheduler scheduler;
Cheat cheat;
#include "video.cpp"
#include "peripherals.cpp"
#include "random.cpp"
#include "serialization.cpp"

auto System::init() -> void {
  vssystem.init();
  playchoice10.init();
  famicombox.init();
}

auto System::load(Emulator::Interface* interface, Model model) -> bool {
  information = {};
  information.model = model;

  if(auto fp = platform->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  auto system = document["system"];

  bus.reset();
  if(!cartridge.load()) return false;

  switch(cartridge.region()) {
  case Cartridge::Region::NTSC:  information.region = Region::NTSC;  break;
  case Cartridge::Region::PAL:   information.region = Region::PAL;   break;
  case Cartridge::Region::Dendy: information.region = Region::Dendy; break;
  }
  if(system["region"].text() == "NTSC" ) information.region = Region::NTSC;
  if(system["region"].text() == "PAL"  ) information.region = Region::PAL;
  if(system["region"].text() == "Dendy") information.region = Region::Dendy;

  if(!cpu.load(system)) return false;
  if(!apu.load(system)) return false;
  if(!ppu.load(system)) return false;

  information.colorburst = region() == Region::NTSC
  ? Emulator::Constants::Colorburst::NTSC
  : Emulator::Constants::Colorburst::PAL;

  switch(model) {

  case Model::Famicom: {
    peripherals.connect(ID::Port::Arcade, ID::Device::None);
    break;
  }

  case Model::VSSystem: {
    vssystem.load();
    peripherals.connect(ID::Port::Arcade, ID::Device::VSPanel);
    break;
  }

  case Model::PlayChoice10: {
    if(!playchoice10.load(system)) return false;
    peripherals.connect(ID::Port::Arcade, ID::Device::PC10Panel);
    break;
  }

  case Model::FamicomBox: {
    if(!famicombox.load(system)) return false;
    peripherals.connect(ID::Port::Arcade, ID::Device::None);
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

  switch(model()) {
  case Model::VSSystem:     vssystem.unload(); break;
  case Model::PlayChoice10: playchoice10.unload(); break;
  case Model::FamicomBox:   famicombox.unload(); break;
  }

  cartridge.unload();
  information.loaded = false;
}

auto System::power() -> void {
  random.seed((uint)time(0));

  cartridge.power();
  cpu.power();
  apu.power();
  ppu.power();

  switch(model()) {
  case Model::VSSystem:     vssystem.power(); break;
  case Model::PlayChoice10: playchoice10.power(); break;
  case Model::FamicomBox:   famicombox.power(); break;
  }

//ppu.reset();
  reset();
}

auto System::reset() -> void {
  Emulator::video.reset();
  Emulator::video.setInterface(interface);
  //Emulator::video.resize() is called in configureVideoEffects()
  configureVideoPalette();
  configureVideoEffects();

  resetAudio();

  scheduler.reset();
  cartridge.reset();
  cpu.reset();
  apu.reset();
  //Only the NES front-loader's PPU will reset. The Famicom's and NES top-loader's PPU will not.
  //Because of a limitation of the Scheduler, the PPU must reset when the system resets.
  ppu.reset();

  switch(model()) {
  case Model::VSSystem:     vssystem.reset(); break;
  case Model::PlayChoice10: playchoice10.reset(); break;
  case Model::FamicomBox:   famicombox.reset(); break;
  }

  switch(model()) {
  case Model::VSSystem:     cpu.coprocessors.append(&vssystem); break;
  case Model::PlayChoice10: cpu.coprocessors.append(&playchoice10.pc10cpu); break;
  case Model::FamicomBox:   cpu.coprocessors.append(&famicombox); break;
  }

  scheduler.primary(cpu);
  peripherals.reset();
}

auto System::resetAudio() -> void {
  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);
}

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) {
    ppu.refresh();
    if(model() == Model::PlayChoice10) playchoice10.videoCircuit.refresh();
  }
}

auto System::runToSave() -> void {
  scheduler.synchronize(cpu);
  scheduler.synchronize(apu);
  scheduler.synchronize(ppu);
  scheduler.synchronize(cartridge);
  for(auto coprocessor : cpu.coprocessors) scheduler.synchronize(*coprocessor);
  for(auto peripheral : cpu.peripherals) scheduler.synchronize(*peripheral);
}

}
