#include <gba/gba.hpp>

namespace GameBoyAdvance {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;

  information.manufacturer = "Nintendo";
  information.name         = "Game Boy Advance";
  information.canvasWidth  = 240;  //note: technically 240x160; but screen can be rotated
  information.canvasHeight = 240;  //by using a square size
  information.overscan     = false;
  information.aspectRatio  = 1.0;
  information.resettable   = false;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::GameBoyAdvance, "Game Boy Advance", "gba", true});

  { Device device{0, ID::DeviceHorizontal, "Controller"};
    device.input.append({ 0, 0, "A"     });
    device.input.append({ 1, 0, "B"     });
    device.input.append({ 2, 0, "Select"});
    device.input.append({ 3, 0, "Start" });
    device.input.append({ 4, 0, "Right" });
    device.input.append({ 5, 0, "Left"  });
    device.input.append({ 6, 0, "Up"    });
    device.input.append({ 7, 0, "Down"  });
    device.input.append({ 8, 0, "R"     });
    device.input.append({ 9, 0, "L"     });
    device.input.append({10, 2, "Rumble"});
    device.order = {6, 7, 5, 4, 1, 0, 9, 8, 2, 3, 10};
    this->device.append(device);
  }

  { Device device{1, ID::DeviceVertical, "Controller"};
    device.input.append({ 0, 0, "A"     });
    device.input.append({ 1, 0, "B"     });
    device.input.append({ 2, 0, "Select"});
    device.input.append({ 3, 0, "Start" });
    device.input.append({ 4, 0, "Up"    });
    device.input.append({ 5, 0, "Down"  });
    device.input.append({ 6, 0, "Left"  });
    device.input.append({ 7, 0, "Right" });
    device.input.append({ 8, 0, "R"     });
    device.input.append({ 9, 0, "L"     });
    device.input.append({10, 2, "Rumble"});
    device.order = {4, 5, 6, 7, 1, 0, 9, 8, 2, 3, 10};
    this->device.append(device);
  }

  port.append({0, "Horizontal Orientation", {device[0]}});
  port.append({1, "Vertical Orientation", {device[1]}});
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoFrequency() -> double {
  return 16777216.0 / (228.0 * 1232.0);
}

auto Interface::audioFrequency() -> double {
  return 16777216.0 / 512.0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::group(uint id) -> uint {
  switch(id) {
  case ID::SystemManifest:
  case ID::BIOS:
    return ID::System;
  case ID::Manifest:
  case ID::MROM:
  case ID::SRAM:
  case ID::EEPROM:
  case ID::FLASH:
    return ID::GameBoyAdvance;
  }

  throw;
}

auto Interface::load(uint id) -> void {
  system.load();
}

auto Interface::save() -> void {
  for(auto& memory : cartridge.memory) {
    interface->saveRequest(memory.id, memory.name);
  }
}

auto Interface::load(uint id, const stream& stream) -> void {
  if(id == ID::SystemManifest) {
    system.information.manifest = stream.text();
  }

  if(id == ID::BIOS) {
    stream.read((uint8_t*)bios.data, min(bios.size, stream.size()));
  }

  if(id == ID::Manifest) {
    cartridge.information.markup = stream.text();
  }

  if(id == ID::MROM) {
    stream.read((uint8_t*)cartridge.mrom.data, min(cartridge.mrom.size, stream.size()));
  }

  if(id == ID::SRAM) {
    stream.read((uint8_t*)cartridge.sram.data, min(cartridge.sram.size, stream.size()));
  }

  if(id == ID::EEPROM) {
    stream.read((uint8_t*)cartridge.eeprom.data, min(cartridge.eeprom.size, stream.size()));
  }

  if(id == ID::FLASH) {
    stream.read((uint8_t*)cartridge.flash.data, min(cartridge.flash.size, stream.size()));
  }
}

auto Interface::save(uint id, const stream& stream) -> void {
  if(id == ID::SRAM) {
    stream.write((uint8_t*)cartridge.sram.data, cartridge.sram.size);
  }

  if(id == ID::EEPROM) {
    stream.write((uint8_t*)cartridge.eeprom.data, cartridge.eeprom.size);
  }

  if(id == ID::FLASH) {
    stream.write((uint8_t*)cartridge.flash.data, cartridge.flash.size);
  }
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::power() -> void {
  system.power();
}

auto Interface::reset() -> void {
  system.power();
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::rotate() -> void {
  system.rotate();
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
      uint mode;
      if(part.size() == 2) {
        if(part[1].size() == 2) mode = Byte;
        if(part[1].size() == 4) mode = Half;
        if(part[1].size() == 8) mode = Word;
        cheat.append(hex(part[0]), hex(part[1]), mode);
      }
      if(part.size() == 3) {
        if(part[1].size() != part[2].size()) continue;
        if(part[2].size() == 2) mode = Byte;
        if(part[2].size() == 4) mode = Half;
        if(part[2].size() == 8) mode = Word;
        cheat.append(hex(part[0]), hex(part[1]), hex(part[2]), mode);
      }
    }
  }
}

auto Interface::cap(const string& name) -> bool {
  if(name == "Blur Emulation") return true;
  if(name == "Color Emulation") return true;
  return false;
}

auto Interface::get(const string& name) -> any {
  if(name == "Blur Emulation") return settings.blurEmulation;
  if(name == "Color Emulation") return settings.colorEmulation;
  return {};
}

auto Interface::set(const string& name, const any& value) -> bool {
  if(name == "Blur Emulation" && value.is<bool>()) {
    settings.blurEmulation = value.get<bool>();
    system.configureVideoEffects();
    return true;
  }

  if(name == "Color Emulation" && value.is<bool>()) {
    settings.colorEmulation = value.get<bool>();
    system.configureVideoPalette();
    return true;
  }

  return false;
}

auto Interface::exportMemory() -> void {
  string pathname = {path(group(ID::MROM)), "debug/"};
  directory::create(pathname);

  file::write({pathname, "i-work.ram"}, (uint8_t*)cpu.iwram, 32 * 1024);
  file::write({pathname, "e-work.ram"}, (uint8_t*)cpu.ewram, 256 * 1024);
  file::write({pathname, "video.ram"}, (uint8_t*)ppu.vram, 96 * 1024);
  uint8_t obj_data[128 * 8];
  for(uint addr = 0; addr < 128 * 8; addr++) {
    obj_data[addr] = ppu.oam_read(Byte, addr | 0x0700'0000);
  }
  file::write({pathname, "sprite.ram"}, obj_data, 128 * 8);
  uint8_t pal_data[256 * 2 * 2];
  for(uint addr = 0; addr < 256 * 2 * 2; addr++) {
    pal_data[addr] = ppu.pram_read(Byte, addr | 0x0500'0000);
  }
  file::write({pathname, "palette.ram"}, pal_data, 256 * 2 * 2);
  if(cartridge.sram.size) saveRequest(ID::SRAM, "debug/save-static.ram");
  if(cartridge.eeprom.size) saveRequest(ID::EEPROM, "debug/save-eeprom.ram");
  if(cartridge.flash.size) saveRequest(ID::FLASH, "debug/save-flash.ram");
}

}
