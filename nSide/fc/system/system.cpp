#include <fc/fc.hpp>

namespace Famicom {

System system;

#include "device.cpp"
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
  //for(auto peripheral : cpu.peripherals) {
  //  scheduler.synchronize(peripheral->thread);
  //}
}

auto System::init() -> void {
  assert(interface != nullptr);

  vssystem.init();
  playchoice10.init();

  device.connect(0, (Device::ID)settings.controllerPort1);
  device.connect(1, (Device::ID)settings.controllerPort2);
  device.connect(2, (Device::ID)settings.expansionPort);
  device.connect(3, Device::ID::None);
}

auto System::term() -> void {
}

auto System::load(Revision revision) -> void {
  _revision = revision;
  interface->loadRequest(ID::SystemManifest, "manifest.bml", true);
  auto document = BML::unserialize(information.manifest);

  if(pc10()) {
    auto rom = document["system/pc10"].find("rom");

    auto firmware = rom(0)["name"].text();
    interface->loadRequest(ID::PC10BIOS, firmware, true);
    if(!file::exists({interface->path(ID::System), firmware})) {
      interface->notify("Error: required PlayChoice-10 firmware ", firmware, " not found.\n");
    }

    auto character = rom(1)["name"].text();
    interface->loadRequest(ID::PC10CharacterROM, character, true);
    if(!file::exists({interface->path(ID::System), character})) {
      interface->notify("Error: required PlayChoice-10 character data ", character, " not found.\n");
    }

    auto palette = rom(2)["name"].text();
    interface->loadRequest(ID::PC10PaletteROM, palette, true);
    if(!file::exists({interface->path(ID::System), palette})) {
      interface->notify("Error: required PlayChoice-10 palette data ", palette, " not found.\n");
    }
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
    break;
  case Revision::VSSystem:
    vssystem.load();
    device.connect(3, Device::ID::VSPanel);
    break;
  case Revision::PlayChoice10:
    interface->information.height = min(max(document["system/pc10/screen/mode"].integer(), 1), 2) * 240;
    playchoice10.load();
    break;
  }

  serializeInit();
  _loaded = true;
}

auto System::unload() -> void {
  if(!loaded()) return;
  switch(revision()) {
  case Revision::VSSystem: vssystem.unload(); break;
  case Revision::PlayChoice10: playchoice10.unload(); break;
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
  case Revision::VSSystem: vssystem.power(); break;
  case Revision::PlayChoice10: playchoice10.power(); break;
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
  case Revision::VSSystem: vssystem.reset(); break;
  case Revision::PlayChoice10: playchoice10.reset(); break;
  }

  switch(revision()) {
  case Revision::VSSystem: cpu.coprocessors.append(&vssystem); break;
  }

  scheduler.reset();
  device.connect(0, (Device::ID)settings.controllerPort1);
  device.connect(1, (Device::ID)settings.controllerPort2);
  device.connect(2, (Device::ID)settings.expansionPort);

  switch(revision()) {
  case Revision::Famicom:      device.connect(3, Device::ID::None);    break;
  case Revision::VSSystem:     device.connect(3, Device::ID::VSPanel); break;
  case Revision::PlayChoice10: device.connect(3, Device::ID::None);    break;
  }
}

}
