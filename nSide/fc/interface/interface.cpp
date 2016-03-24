#include <fc/fc.hpp>

namespace Famicom {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  system.init();

  information.manufacturer = "Nintendo";
  information.name         = "Famicom";
  information.width        = 256;
  information.height       = 240;
  information.overscan     = true;
  information.aspectRatio  = 8.0 / 7.0;
  information.resettable   = true;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::Famicom,      "Famicom",       "fc",   true});
  media.append({ID::VSSystem,     "VS. System",    "vs",   true});
  media.append({ID::PlayChoice10, "PlayChoice-10", "pc10", true});

  port.append({0, "Port 1"});
  port.append({1, "Port 2"});
  port.append({2, "Expansion Port"});
  port.append({3, "Arcade Panel"});

  { Device device{
      (uint)Famicom::Device::ID::None,
      ID::ControllerPort1 | ID::ControllerPort2 | ID::ExpansionPort,
      "None"
    };
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::Gamepad,
      ID::ControllerPort1 | ID::ControllerPort2 | ID::ExpansionPort,
      "Gamepad"
    };
    device.input.append({0, 0, "A"     });
    device.input.append({1, 0, "B"     });
    device.input.append({2, 0, "Select"});
    device.input.append({3, 0, "Start" });
    device.input.append({4, 0, "Up"    });
    device.input.append({5, 0, "Down"  });
    device.input.append({6, 0, "Left"  });
    device.input.append({7, 0, "Right" });
    device.order = {4, 5, 6, 7, 1, 0, 2, 3};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::FourPlayers,
      ID::ExpansionPort,
      "4-Players Adaptor"
    };
    for(uint p = 3, n = 0; p <= 4; p += 1, n += 8) {
      device.input.append({n + 0, 0, {"Port ", p, " - ", "A"     }});
      device.input.append({n + 1, 0, {"Port ", p, " - ", "B"     }});
      device.input.append({n + 2, 0, {"Port ", p, " - ", "Select"}});
      device.input.append({n + 3, 0, {"Port ", p, " - ", "Start" }});
      device.input.append({n + 4, 0, {"Port ", p, " - ", "Up"    }});
      device.input.append({n + 5, 0, {"Port ", p, " - ", "Down"  }});
      device.input.append({n + 6, 0, {"Port ", p, " - ", "Left"  }});
      device.input.append({n + 7, 0, {"Port ", p, " - ", "Right" }});
      device.order.append(n + 4, n + 5, n + 6, n + 7);
      device.order.append(n + 1, n + 0, n + 2, n + 3);
    }
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::FourScore,
      ID::ControllerPort1,
      "Four Score"
    };
    for(uint p = 1, n = 0; p <= 4; p += 2, n += 8) {
      device.input.append({n + 0, 0, {"Port ", p, " - ", "A"     }});
      device.input.append({n + 1, 0, {"Port ", p, " - ", "B"     }});
      device.input.append({n + 2, 0, {"Port ", p, " - ", "Select"}});
      device.input.append({n + 3, 0, {"Port ", p, " - ", "Start" }});
      device.input.append({n + 4, 0, {"Port ", p, " - ", "Up"    }});
      device.input.append({n + 5, 0, {"Port ", p, " - ", "Down"  }});
      device.input.append({n + 6, 0, {"Port ", p, " - ", "Left"  }});
      device.input.append({n + 7, 0, {"Port ", p, " - ", "Right" }});
      device.order.append(n + 4, n + 5, n + 6, n + 7);
      device.order.append(n + 1, n + 0, n + 2, n + 3);
    }
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::FourScore,
      ID::ControllerPort2,
      "Four Score"
    };
    for(uint p = 2, n = 0; p <= 4; p += 2, n += 8) {
      device.input.append({n + 0, 0, {"Port ", p, " - ", "A"     }});
      device.input.append({n + 1, 0, {"Port ", p, " - ", "B"     }});
      device.input.append({n + 2, 0, {"Port ", p, " - ", "Select"}});
      device.input.append({n + 3, 0, {"Port ", p, " - ", "Start" }});
      device.input.append({n + 4, 0, {"Port ", p, " - ", "Up"    }});
      device.input.append({n + 5, 0, {"Port ", p, " - ", "Down"  }});
      device.input.append({n + 6, 0, {"Port ", p, " - ", "Left"  }});
      device.input.append({n + 7, 0, {"Port ", p, " - ", "Right" }});
      device.order.append(n + 4, n + 5, n + 6, n + 7);
      device.order.append(n + 1, n + 0, n + 2, n + 3);
    }
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::BeamGun,
      ID::ExpansionPort,
      "Beam Gun"
    };
    device.input.append({0, 1, "X-axis" });
    device.input.append({1, 1, "Y-axis" });
    device.input.append({2, 0, "Trigger"});
    device.order = {0, 1, 2};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::BeamGun,
      ID::ControllerPort2,
      "Zapper"
    };
    device.input.append({0, 1, "X-axis" });
    device.input.append({1, 1, "Y-axis" });
    device.input.append({2, 0, "Trigger"});
    device.order = {0, 1, 2};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::FamilyTrainer,
      ID::ExpansionPort,
      "Family Trainer"
    };
    for(uint n = 0; n <= 11; n++) {
      device.input.append({n, 0, {"Button ", n + 1}});
    }
    device.order = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::FamilyTrainer,
      ID::ControllerPort2,
      "Power Pad"
    };
    for(uint n = 0; n <= 11; n++) {
      device.input.append({n, 0, {"Button ", n + 1}});
    }
    device.order = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::Vaus,
      ID::ControllerPort2 | ID::ExpansionPort,
      "Arkanoid Vaus"
    };
    device.input.append({0, 1, "Control Knob"});
    device.input.append({2, 0, "Fire Button" });
    device.order = {0, 1};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::SFCGamepad,
      ID::ControllerPort1 | ID::ControllerPort2,
      "SFC Gamepad"
    };
    device.input.append({ 0, 0, "B"     });
    device.input.append({ 1, 0, "Y"     });
    device.input.append({ 2, 0, "Select"});
    device.input.append({ 3, 0, "Start" });
    device.input.append({ 4, 0, "Up"    });
    device.input.append({ 5, 0, "Down"  });
    device.input.append({ 6, 0, "Left"  });
    device.input.append({ 7, 0, "Right" });
    device.input.append({ 8, 0, "A"     });
    device.input.append({ 9, 0, "X"     });
    device.input.append({10, 0, "L"     });
    device.input.append({11, 0, "R"     });
    device.order = {4, 5, 6, 7, 0, 8, 1, 9, 10, 11, 2, 3};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::Mouse,
      ID::ControllerPort1 | ID::ControllerPort2 | ID::ExpansionPort,
      "Mouse"
    };
    device.input.append({0, 1, "X-axis"});
    device.input.append({1, 1, "Y-axis"});
    device.input.append({2, 0, "Left"  });
    device.input.append({3, 0, "Right" });
    device.order = {0, 1, 2, 3};
    addDevice(device);
  }

  { Device device{
      (uint)Famicom::Device::ID::VSPanel,
      ID::ArcadePanel,
      "VS. Panel"
    };
    device.input.append({0, 0, "Button 1"      });
    device.input.append({1, 0, "Button 2"      });
    device.input.append({2, 0, "Button 3"      });
    device.input.append({3, 0, "Button 4"      });
    device.input.append({4, 0, "Service Button"});
    device.input.append({5, 0, "Coin 1"        });
    device.input.append({6, 0, "Coin 2"        });
    device.order = {0, 1, 2, 3, 4, 5, 6};
    addDevice(device);
  }
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC: return system.cpuFrequency() / (262.0 * 1364.0 - 4.0);
  case System::Region::PAL:  return system.cpuFrequency() / (312.0 * 1705.0);
  }
}

auto Interface::audioFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC: return system.cpuFrequency() / 12.0;
  case System::Region::PAL:  return system.cpuFrequency() / 16.0;
  }
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::group(uint id) -> uint {
  switch(id) {
  case ID::SystemManifest:
  case ID::PC10BIOS:
  case ID::PC10CharacterROM:
  case ID::PC10PaletteROM:
    return ID::System;
  case ID::Manifest:
  case ID::ProgramROM:
  case ID::ProgramRAM:
  case ID::CharacterROM:
  case ID::CharacterRAM:
  case ID::ChipRAM:
    switch(system.revision()) {
    case System::Revision::Famicom:      return ID::Famicom;
    case System::Revision::VSSystem:     return ID::VSSystem;
    case System::Revision::PlayChoice10: return ID::PlayChoice10;
    }
  case ID::InstructionROM:
  case ID::KeyROM:
    return ID::PlayChoice10;
  }

  throw;
}

auto Interface::load(uint id) -> void {
  information.width  = 256;
  information.height = 240;
  if(id == ID::Famicom) system.load(System::Revision::Famicom);
  if(id == ID::VSSystem) system.load(System::Revision::VSSystem); 
  if(id == ID::PlayChoice10) system.load(System::Revision::PlayChoice10);
}

auto Interface::save() -> void {
  for(auto& memory : cartridge.memory) {
    saveRequest(memory.id, memory.name);
  }
}

auto Interface::load(uint id, const stream& stream) -> void {
  switch(id) {
  case ID::SystemManifest:
    system.information.manifest = stream.text();
    break;
  case ID::PC10BIOS:
    stream.read((uint8_t*)playchoice10.bios, min(16384u, stream.size()));
    break;
  case ID::PC10CharacterROM:
    stream.read((uint8_t*)playchoice10.chrrom, min(24576u, stream.size()));
    break;
  case ID::PC10PaletteROM:
    stream.read((uint8_t*)playchoice10.cgrom, min(768u, stream.size()));
    break;
  }

  if(id == ID::Manifest) cartridge.information.markup.cartridge = stream.text();
  if(id == ID::ProgramROM) cartridge.board->prgrom.read(stream);
  if(id == ID::ProgramRAM) cartridge.board->prgram.read(stream);
  if(id == ID::CharacterROM) cartridge.board->chrrom.read(stream);
  if(id == ID::CharacterRAM) cartridge.board->chrram.read(stream);
  if(id == ID::ChipRAM) cartridge.board->chip->ram.read(stream);

  if(id == ID::InstructionROM) cartridge.board->instrom.read(stream);
  if(id == ID::KeyROM) cartridge.board->keyrom.read(stream);
}

auto Interface::save(uint id, const stream& stream) -> void {
  if(id == ID::ProgramRAM) {
    stream.write((uint8_t*)cartridge.board->prgram.data(), cartridge.board->prgram.size());
  }

  if(id == ID::CharacterRAM) {
    stream.write((uint8_t*)cartridge.board->chrram.data(), cartridge.board->chrram.size());
  }

  if(id == ID::ChipRAM) {
    stream.write((uint8_t*)cartridge.board->chip->ram.data(), cartridge.board->chip->ram.size());
  }
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  if(system.vs() || port == (uint)Famicom::Device::Port::ArcadePanel) return;
  Famicom::device.connect(port, (Famicom::Device::ID)device);
}

auto Interface::power() -> void {
  system.power();
}

auto Interface::reset() -> void {
  system.reset();
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::serialize() -> serializer {
  system.runToSave();
  return system.serialize();
}

auto Interface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto Interface::cheatSet(const lstring& list) -> void {
  cheat.reset();

  for(auto& codeset : list) {
    lstring codes = codeset.split("+");
    for(auto& code : codes) {
      lstring part = code.split("/");
      if(part.size() == 2) cheat.append(hex(part[0]), hex(part[1]));
      if(part.size() == 3) cheat.append(hex(part[0]), hex(part[1]), hex(part[2]));
    }
  }
}

auto Interface::cap(const string& name) -> bool {
  if(name == "Color Emulation") return true;
  if(name == "Scanline Emulation") return true;
  return false;
}

auto Interface::get(const string& name) -> any {
  if(name == "Color Emulation") return settings.colorEmulation;
  if(name == "Scanline Emulation") return settings.scanlineEmulation;
  return {};
}

auto Interface::set(const string& name, const any& value) -> bool {
  if(name == "Color Emulation" && value.is<bool>()) return settings.colorEmulation = value.get<bool>(), true;
  if(name == "Scanline Emulation" && value.is<bool>()) return settings.scanlineEmulation = value.get<bool>(), true;
  return false;
}

auto Interface::addDevice(Device device) -> void {
  while(device_ref.size() <= device.id) {
    device_ref.append(DeviceRef{device_ref.size(), 0, 0, 0, 0});
  }
  for(auto& port : this->port) {
    if(device.portmask & (1 << port.id)) {
      device_ref[device.id].port[port.id] = (uint)port.device.size();
      port.device.append(device);
    }
  }
}

auto Interface::exportMemory() -> void {
  string pathname = {path(group(ID::ProgramROM)), "debug/"};
  directory::create(pathname);

  // Registers
  string markup = "";
  ppu.exportRegisters(markup);
  file::write({pathname, "registers.bml"}, markup);

  file::write({pathname, "work.ram"}, (uint8_t*)cpu.ram, 0x0800);
  file::write({pathname, "video.ram"}, (uint8_t*)ppu.ciram, !system.vs() ? 2048 : 4096);
  file::write({pathname, "sprite.ram"}, (uint8_t*)ppu.oam, 256);
  file::write({pathname, "palette.ram"}, (uint8_t*)ppu.cgram, 32);
  if(cartridge.board->prgram.size()) saveRequest(ID::ProgramRAM, "debug/program.ram");
  if(cartridge.board->chrram.size()) saveRequest(ID::CharacterRAM, "debug/character.ram");
  if(cartridge.board->chip && cartridge.board->chip->ram.size()) {
    saveRequest(ID::ChipRAM, "debug/chip.ram");
  }
}

}
