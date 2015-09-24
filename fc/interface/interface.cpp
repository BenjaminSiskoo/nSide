#include <fc/fc.hpp>

namespace Famicom {

Interface* interface = nullptr;

Interface::Interface() {
  interface = this;
  system.init();

  information.name        = "Famicom";
  information.width       = 256;
  information.height      = 240;
  information.overscan    = true;
  information.aspectRatio = 8.0 / 7.0;
  information.resettable  = true;
  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::Famicom,      "Famicom",       "fc",   true});
  media.append({ID::VSSystem,     "VS. System",    "vs",   true});
  media.append({ID::PlayChoice10, "PlayChoice-10", "pc10", true});

  port.append({0, "Port 1"});
  port.append({1, "Port 2"});
  port.append({2, "Expansion Port"});

  for(unsigned i = 0; i <= (unsigned)Input::Device::None; i++) {
    device_ref.append(DeviceRef{i, 0, 0, 0});
  }

  { Device device{
      (unsigned)Input::Device::Joypad,
      ID::Port1 | ID::Port2 | ID::ExpansionPort,
      "Controller"
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
      (unsigned)Input::Device::FourPlayers,
      ID::ExpansionPort,
      "4-Players Adaptor"
    };
    for(unsigned p = 3, n = 0; p <= 4; p += 1, n += 8) {
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
      (unsigned)Input::Device::FourScore,
      ID::Port1,
      "Four Score"
    };
    for(unsigned p = 1, n = 0; p <= 4; p += 2, n += 8) {
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
      (unsigned)Input::Device::FourScore,
      ID::Port2,
      "Four Score"
    };
    for(unsigned p = 2, n = 0; p <= 4; p += 2, n += 8) {
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
      (unsigned)Input::Device::BeamGun,
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
      (unsigned)Input::Device::BeamGun,
      ID::Port2,
      "Zapper"
    };
    device.input.append({0, 1, "X-axis" });
    device.input.append({1, 1, "Y-axis" });
    device.input.append({2, 0, "Trigger"});
    device.order = {0, 1, 2};
    addDevice(device);
  }

  { Device device{
      (unsigned)Input::Device::FamilyTrainer,
      ID::ExpansionPort,
      "Family Trainer"
    };
    for(unsigned n = 0; n <= 11; n++) {
      device.input.append({n, 0, {"Button ", n + 1}});
    }
    device.order = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    addDevice(device);
  }

  { Device device{
      (unsigned)Input::Device::FamilyTrainer,
      ID::Port2,
      "Power Pad"
    };
    for(unsigned n = 0; n <= 11; n++) {
      device.input.append({n, 0, {"Button ", n + 1}});
    }
    device.order = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    addDevice(device);
  }

  { Device device{
      (unsigned)Input::Device::SFCJoypad,
      ID::Port1 | ID::Port2,
      "SFC Controller"
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
      (unsigned)Input::Device::Mouse,
      ID::Port1 | ID::Port2 | ID::ExpansionPort,
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
      (unsigned)Input::Device::VSPanel,
      ID::ExpansionPort,
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

  { Device device{
      (unsigned)Input::Device::None,
      ID::Port1 | ID::Port2 | ID::ExpansionPort,
      "None"
    };
    addDevice(device);
  }
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC: return system.cpu_frequency() / (262.0 * 1364.0 - 4.0);
  case System::Region::PAL:  return system.cpu_frequency() / (312.0 * 1705.0);
  }
}

auto Interface::audioFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC: return system.cpu_frequency() / 12.0;
  case System::Region::PAL:  return system.cpu_frequency() / 16.0;
  }
}

auto Interface::loaded() -> bool {
  return cartridge.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::group(unsigned id) -> unsigned {
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
    switch(system.revision) {
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

auto Interface::load(unsigned id) -> void {
  information.width  = 256;
  information.height = 240;
  switch(id) {
  case ID::Famicom:      cartridge.load(System::Revision::Famicom);      break;
  case ID::VSSystem:     cartridge.load(System::Revision::VSSystem);     break;
  case ID::PlayChoice10: cartridge.load(System::Revision::PlayChoice10); break;
  }
}

auto Interface::save() -> void {
  for(auto& memory : cartridge.memory) {
    saveRequest(memory.id, memory.name);
  }
}

auto Interface::load(unsigned id, const stream& stream) -> void {
  switch(id) {
  case ID::SystemManifest:
    system.information.manifest = stream.text();
    break;
  case ID::PC10BIOS:
    stream.read(pc10arcadeboard.bios, min(16384u, stream.size()));
    break;
  case ID::PC10CharacterROM:
    stream.read(pc10arcadeboard.chrrom, min(24576u, stream.size()));
    break;
  case ID::PC10PaletteROM:
    stream.read(pc10arcadeboard.cgrom, min(768u, stream.size()));
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

auto Interface::save(unsigned id, const stream& stream) -> void {
  if(id == ID::ProgramRAM) {
    stream.write(cartridge.board->prgram.data(), cartridge.board->prgram.size());
  }

  if(id == ID::CharacterRAM) {
    stream.write(cartridge.board->chrram.data(), cartridge.board->chrram.size());
  }

  if(id == ID::ChipRAM) {
    stream.write(cartridge.board->chip->ram.data(), cartridge.board->chip->ram.size());
  }
}

auto Interface::unload() -> void {
  save();
  cartridge.unload();
}

auto Interface::connect(unsigned port, unsigned device) -> void {
  if(!system.vs()) {
    // Don't allow switching to VS. Panel
    if(device == (unsigned)Input::Device::VSPanel)
      device = (unsigned)Input::Device::None;
    input.connect(port, (Input::Device)device);
  }
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
  system.runtosave();
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

auto Interface::paletteUpdate(PaletteMode mode) -> void {
  video.generate_palette(mode);
}

auto Interface::addDevice(Device device) -> void {
  for(auto& port : this->port) {
    if(device.portmask & (1 << port.id)) {
      device_ref[device.id].port[port.id] = (unsigned)port.device.size();
      port.device.append(device);
    }
  }
}

auto Interface::exportMemory() -> void {
  string pathname = {path(group(ID::ProgramROM)), "debug/"};
  directory::create(pathname);

  // Registers
  string markup = "";
  markup.append("ppu\n");
  markup.append("  vaddr:       0x", hex(ppu.status.vaddr, 4L), "\n");
  markup.append("  taddr:       0x", hex(ppu.status.taddr, 4L), "\n");
  markup.append("  xaddr:       0x", hex(ppu.status.xaddr, 1L), "\n");
  markup.append("  sprite-size: ",   ppu.status.sprite_size ? "8x16" : "8x8", "\n");
  markup.append("  bg-addr:     0x", hex(ppu.status.bg_addr, 4L), "\n");
  markup.append("  sprite-addr: 0x", hex(ppu.status.sprite_addr, 4L), "\n");
  file::write({pathname, "registers.bml"}, markup);

  file::write({pathname, "work.ram"}, cpu.ram, 0x0800);
  file::write({pathname, "video.ram"}, ppu.ciram, !system.vs() ? 2048 : 4096);
  file::write({pathname, "sprite.ram"}, ppu.oam, 256);
  file::write({pathname, "palette.ram"}, ppu.cgram, 32);
  if(cartridge.board->prgram.size()) saveRequest(ID::ProgramRAM, "debug/program.ram");
  if(cartridge.board->chrram.size()) saveRequest(ID::CharacterRAM, "debug/character.ram");
  if(cartridge.board->chip && cartridge.board->chip->ram.size())
    saveRequest(ID::ChipRAM, "debug/chip.ram");
}

}
