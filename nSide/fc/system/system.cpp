#include <fc/fc.hpp>

namespace Famicom {

System system;
Configuration configuration;

#include "video.cpp"
#include "device.cpp"
#include "serialization.cpp"

#include <fc/scheduler/scheduler.cpp>

System::System() {
  region = Region::Autodetect;
}

auto System::run() -> void {
  scheduler.sync = Scheduler::SynchronizeMode::None;

  scheduler.enter();
  if(scheduler.exit_reason == Scheduler::ExitReason::FrameEvent) {
    video.refresh();
  }
}

auto System::runToSave() -> void {
  scheduler.sync = Scheduler::SynchronizeMode::PPU;
  runThreadToSave();

  scheduler.sync = Scheduler::SynchronizeMode::All;
  scheduler.thread = cpu.thread;
  runThreadToSave();

  scheduler.sync = Scheduler::SynchronizeMode::All;
  scheduler.thread = apu.thread;
  runThreadToSave();

  scheduler.sync = Scheduler::SynchronizeMode::All;
  scheduler.thread = cartridge.thread;
  runThreadToSave();

  scheduler.sync = Scheduler::SynchronizeMode::None;
}

auto System::runThreadToSave() -> void {
  while(true) {
    scheduler.enter();
    if(scheduler.exit_reason == Scheduler::ExitReason::SynchronizeEvent) break;
    if(scheduler.exit_reason == Scheduler::ExitReason::FrameEvent) {
      video.refresh();
    }
  }
}

auto System::init() -> void {
  assert(interface != nullptr);

  vsarcadeboard.init();
  pc10arcadeboard.init();

  device.connect(0, configuration.controllerPort1);
  device.connect(1, configuration.controllerPort2);
  device.connect(2, configuration.expansionPort);
}

auto System::term() -> void {
}

auto System::load(Revision revision) -> void {
  this->revision = revision;
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

  region = configuration.region;
  if(region == Region::Autodetect) {
    region = (cartridge.region() == Cartridge::Region::NTSC ? Region::NTSC : Region::PAL);
  }

  cpuFrequency = region() == Region::NTSC ? 21477272 : 26601712;

  if(!vs()) { // VS. System PPU is set within cartridge.load().
    auto game_manifest = BML::unserialize(cartridge.information.markup.cartridge);
    if(!game_manifest["board/pc10"]) {
      // most Famicoms use a PPU with open bus OAMDATA (read).
      // For now, we use an NES PPU where OAMDATA (read) is defined.
      if(region == Region::NTSC) ppu.revision = PPU::Revision::RP2C02G;
      if(region == Region::PAL)  ppu.revision = PPU::Revision::RP2C07;
    } else {
      ppu.revision = PPU::Revision::RP2C03B;
    }
  }

  switch(revision) {
  case Revision::Famicom:
    break;
  case Revision::VSSystem:
    vsarcadeboard.load();
    device.connect(2, Device::ID::VSPanel);
    break;
  case Revision::PlayChoice10:
    interface->information.height = min(max(document["system/pc10/screen/mode"].integer(), 1), 2) * 240;
    pc10arcadeboard.load();
    break;
  }

  serializeInit();
}

auto System::unload() -> void {
  switch(revision) {
  case Revision::VSSystem: vsarcadeboard.unload(); break;
  case Revision::PlayChoice10: pc10arcadeboard.unload(); break;
  }
}

auto System::power() -> void {
  cartridge.power();
  cpu.power();
  apu.power();
  ppu.power();

  switch(revision) {
  case Revision::VSSystem: vsarcadeboard.power(); break;
  case Revision::PlayChoice10: pc10arcadeboard.power(); break;
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

  switch(revision) {
  case Revision::VSSystem: vsarcadeboard.reset(); break;
  case Revision::PlayChoice10: pc10arcadeboard.reset(); break;
  }

  video.reset();
  scheduler.init();
  device.connect(0, configuration.controllerPort1);
  device.connect(1, configuration.controllerPort2);
  device.connect(2, configuration.expansionPort);
}

}
