#include <ms/ms.hpp>

namespace MasterSystem {

System system;
Scheduler scheduler;
#include "peripherals.cpp"

auto System::run() -> void {
  if(scheduler.enter() == Scheduler::Event::Frame) {
    static uint32 output[256 * 192] = {0};
    Emulator::video.refresh(output, 256 * sizeof(uint32), 256, 192);
  }
}

auto System::init() -> void {
  assert(interface != nullptr);
}

auto System::load(Model model) -> bool {
  information = Information();
  information.model = model;

  if(auto fp = interface->open(ID::System, "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;
  auto document = BML::unserialize(information.manifest);
  if(!cartridge.load()) return false;
  information.colorburst = Emulator::Constants::Colorburst::NTSC;
  return information.loaded = true;
}

auto System::save() -> void {
  cartridge.save();
}

auto System::unload() -> void {
  cartridge.unload();
}

auto System::power() -> void {
  cartridge.power();
  cpu.power();
  vdp.power();
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
  vdp.reset();
  scheduler.primary(cpu);
}

}
