#include <fc/fc.hpp>

namespace Famicom {

System system;

#include "peripherals.cpp"
#include "serialization.cpp"

auto System::loaded() const -> bool { return _loaded; }
auto System::revision() const -> Revision { return _revision; }
auto System::region() const -> Region { return _region; }
auto System::cpuFrequency() const -> uint { return _cpuFrequency; }

auto System::run() -> void {
  scheduler.enter();
}

auto System::runToSave() -> void {
  scheduler.synchronize(ppu.thread);
  scheduler.synchronize(cpu.thread);
  scheduler.synchronize(apu.thread);
  scheduler.synchronize(cartridge.thread);
  for(auto coprocessor : cpu.coprocessors) {
    scheduler.synchronize(coprocessor->thread);
  }
  for(auto peripheral : cpu.peripherals) {
    scheduler.synchronize(peripheral->thread);
  }
}

auto System::init() -> void {
  assert(interface != nullptr);

  vssystem.init();
  playchoice10.init();
  famicombox.init();
}

auto System::term() -> void {
}

auto System::load(Revision revision) -> void {
  bus.reset();

  _revision = revision;
  interface->loadRequest(ID::SystemManifest, "manifest.bml", true);
  auto document = BML::unserialize(information.manifest);

  if(pc10()) {
    auto firmware = document["system/pc10/cpu/rom/name"].text();
    interface->loadRequest(ID::PC10BIOS, firmware, true);

    auto character = document["system/pc10/video-circuit/vrom/name"].text();
    interface->loadRequest(ID::PC10CharacterROM, character, true);

    auto palette = document["system/pc10/video-circuit/cgrom/name"].text();
    interface->loadRequest(ID::PC10PaletteROM, palette, true);
  }

  cartridge.load();
  _region = cartridge.region() == Cartridge::Region::NTSC ? Region::NTSC : Region::PAL;
  _cpuFrequency = region() == Region::NTSC ? 21'477'272 : 26'601'712;

  if(!vs()) { // VS. System PPU is set within cartridge.load().
    auto game_manifest = BML::unserialize(cartridge.information.markup.cartridge);
    if(!game_manifest["board/pc10"]) {
      // most Famicoms use a PPU with open bus OAMDATA (read).
      // For now, we use an NES PPU where OAMDATA (read) is defined.
      if(region() == Region::NTSC) ppu.revision = PPU::Revision::RP2C02G;
      if(region() == Region::PAL)  ppu.revision = PPU::Revision::RP2C07;
    } else {
      ppu.revision = PPU::Revision::RP2C03B;
    }
  }

  switch(revision) {
  case Revision::Famicom:
    peripherals.connect(3, Device::None);
    break;
  case Revision::VSSystem:
    vssystem.load();
    peripherals.connect(3, Device::VSPanel);
    break;
  case Revision::PlayChoice10:
    interface->information.height = min(max(document["system/pc10/screen/mode"].integer(), 1), 2) * 240;
    playchoice10.load();
    peripherals.connect(3, Device::None);
    break;
  case Revision::FamicomBox:
    famicombox.load();
    peripherals.connect(3, Device::None);
    break;
  }

  serializeInit();
  _loaded = true;
}

auto System::unload() -> void {
  if(!loaded()) return;
  peripherals.unload();

  switch(revision()) {
  case Revision::VSSystem:     vssystem.unload(); break;
  case Revision::PlayChoice10: playchoice10.unload(); break;
  case Revision::FamicomBox:   famicombox.unload(); break;
  }

  cartridge.unload();
  _loaded = false;
}

auto System::power() -> void {
  cartridge.power();
  cpu.power();
  apu.power();
  ppu.power();

  switch(revision()) {
  case Revision::VSSystem:     vssystem.power(); break;
  case Revision::PlayChoice10: playchoice10.power(); break;
  case Revision::FamicomBox:   famicombox.power(); break;
  }

  ppu.reset();
  reset();
}

auto System::reset() -> void {
  cartridge.reset();
  cpu.reset();
  apu.reset();
  // Only the NES front-loader's PPU will reset. The Famicom's and NES
  // top-loader's PPU will not.
  //ppu.reset();

  switch(revision()) {
  case Revision::VSSystem:     vssystem.reset(); break;
  case Revision::PlayChoice10: playchoice10.reset(); break;
  case Revision::FamicomBox:   famicombox.reset(); break;
  }

  switch(revision()) {
  case Revision::VSSystem:     cpu.coprocessors.append(&vssystem); break;
  case Revision::PlayChoice10: cpu.coprocessors.append(&playchoice10.pc10cpu); break;
  case Revision::FamicomBox:   cpu.coprocessors.append(&famicombox); break;
  }

  scheduler.reset();
  peripherals.reset();
}

}
