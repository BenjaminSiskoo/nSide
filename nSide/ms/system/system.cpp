#include <ms/ms.hpp>

namespace MasterSystem {

#include "peripherals.cpp"
System system;
Scheduler scheduler;

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) vdp.refresh();
}

auto System::init() -> void {
  assert(interface != nullptr);
}

auto System::load(Model model) -> bool {
  information = {};
  information.model = model;

  if(auto fp = interface->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  bus.ramMask = system.model() == Model::SG1000 ? 0x3ff : 0x1fff;

  auto document = BML::unserialize(information.manifest);
  if(!cartridge.load()) return false;

  information.colorburst = Emulator::Constants::Colorburst::NTSC;

  peripherals.connect(ID::Port::Hardware, model != Model::GameGear ? ID::Device::Controls : ID::Device::None);

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
  vdp.power();
  psg.power();
  reset();
}

auto System::reset() -> void {
  Emulator::video.reset();
  Emulator::video.setInterface(interface);
  Emulator::video.setPalette();

  Emulator::audio.reset();
  Emulator::audio.setInterface(interface);

  scheduler.reset();
  bus.reset();
  cartridge.reset();
  cpu.reset();
  vdp.reset();
  psg.reset();
  scheduler.primary(cpu);

  peripherals.reset();
}

}
