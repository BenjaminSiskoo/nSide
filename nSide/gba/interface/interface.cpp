#include <gba/gba.hpp>

namespace GameBoyAdvance {

Settings settings;

Interface::Interface() {
  information.devState     = DevState::PreAlpha;
  information.manufacturer = "Nintendo";
  information.name         = "Game Boy Advance";
  information.overscan     = false;

  media.append({ID::GameBoyAdvance, "Game Boy Advance", "gba"});

  Port hardwarePort{ID::Port::Hardware, "Hardware"};

  { Device device{ID::Device::Controls, "Controls"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    device.inputs.append({0, "L"     });
    device.inputs.append({0, "R"     });
    device.inputs.append({0, "Select"});
    device.inputs.append({0, "Start" });
    device.inputs.append({2, "Rumble"});
    hardwarePort.devices.append(device);
  }

  ports.append(move(hardwarePort));
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoResolution() -> VideoResolution {
  if(!settings.rotateLeft) {
    return {240, 160, 240, 160, 1.0};
  } else {
    return {160, 240, 160, 240, 1.0};
  }
}

auto Interface::videoColors() -> uint32 {
  return 1 << 15;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint R = color.bits( 0, 4);
  uint G = color.bits( 5, 9);
  uint B = color.bits(10,14);

  uint64 r = image::normalize(R, 5, 16);
  uint64 g = image::normalize(G, 5, 16);
  uint64 b = image::normalize(B, 5, 16);

  if(settings.colorEmulation) {
    double lcdGamma = 4.0, outGamma = 2.2;
    double lb = pow(B / 31.0, lcdGamma);
    double lg = pow(G / 31.0, lcdGamma);
    double lr = pow(R / 31.0, lcdGamma);
    r = pow((  0 * lb +  50 * lg + 255 * lr) / 255, 1 / outGamma) * (0xffff * 255 / 280);
    g = pow(( 30 * lb + 230 * lg +  10 * lr) / 255, 1 / outGamma) * (0xffff * 255 / 280);
    b = pow((220 * lb +  10 * lg +  50 * lr) / 255, 1 / outGamma) * (0xffff * 255 / 280);
  }

  return r << 32 | g << 16 | b << 0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::load(uint id) -> bool {
  return system.load(this);
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::power() -> void {
  system.power();
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

auto Interface::cheatSet(const string_vector& list) -> void {
  cheat.assign(list);
}

auto Interface::cap(const string& name) -> bool {
  if(name == "Blur Emulation") return true;
  if(name == "Color Emulation") return true;
  if(name == "Rotate Display") return true;
  return false;
}

auto Interface::get(const string& name) -> any {
  if(name == "Blur Emulation") return settings.blurEmulation;
  if(name == "Color Emulation") return settings.colorEmulation;
  if(name == "Rotate Display") return settings.rotateLeft;
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

  if(name == "Rotate Display" && value.is<bool>()) {
    settings.rotateLeft = value.get<bool>();
    system.configureVideoEffects();
    return true;
  }

  return false;
}

auto Interface::exportMemory() -> void {
  string pathname = {platform->path(cartridge.pathID()), "debug/"};
  directory::create(pathname);

  if(auto fp = platform->open(cartridge.pathID(), "debug/i-work.ram", File::Write)) fp->write(cpu.iwram, 32 * 1024);
  if(auto fp = platform->open(cartridge.pathID(), "debug/e-work.ram", File::Write)) fp->write(cpu.ewram, 256 * 1024);
  if(cartridge.sram.size) if(auto fp = platform->open(cartridge.pathID(), "debug/save-static.ram", File::Write)) {
    fp->write(cartridge.sram.data, cartridge.sram.size);
  }
  if(cartridge.eeprom.size) if(auto fp = platform->open(cartridge.pathID(), "debug/save-eeprom.ram", File::Write)) {
    fp->write(cartridge.eeprom.data, cartridge.eeprom.size);
  }
  if(cartridge.flash.size) if(auto fp = platform->open(cartridge.pathID(), "debug/save-flash.ram", File::Write)) {
    fp->write(cartridge.flash.data, cartridge.flash.size);
  }
}

}
