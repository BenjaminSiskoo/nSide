#include <ws/ws.hpp>

namespace WonderSwan {

Settings settings;
#include "wonderswan.cpp"
#include "wonderswan-color.cpp"

Interface::Interface() {
  Port hardware{ID::Port::Hardware, "Hardware"};

  { Device device{ID::Device::HorizontalControls, "Horizontal Controls"};
    device.inputs.append({0, "Y1"});
    device.inputs.append({0, "Y2"});
    device.inputs.append({0, "Y3"});
    device.inputs.append({0, "Y4"});
    device.inputs.append({0, "X1"});
    device.inputs.append({0, "X2"});
    device.inputs.append({0, "X3"});
    device.inputs.append({0, "X4"});
    device.inputs.append({0, "B"});
    device.inputs.append({0, "A"});
    device.inputs.append({0, "Start"});
    hardware.devices.append(device);
  }

  { Device device{ID::Device::VerticalControls, "Vertical Controls"};
    device.inputs.append({0, "Y1"});
    device.inputs.append({0, "Y2"});
    device.inputs.append({0, "Y3"});
    device.inputs.append({0, "Y4"});
    device.inputs.append({0, "X1"});
    device.inputs.append({0, "X2"});
    device.inputs.append({0, "X3"});
    device.inputs.append({0, "X4"});
    device.inputs.append({0, "B"});
    device.inputs.append({0, "A"});
    device.inputs.append({0, "Start"});
    hardware.devices.append(device);
  }

  ports.append(move(hardware));
}

auto Interface::manifest() -> string {
  return cartridge.information.manifest;
}

auto Interface::title() -> string {
  return cartridge.information.title;
}

auto Interface::videoSize() -> VideoSize {
  return {224, 224};
}

auto Interface::videoSize(uint width, uint height, bool arc, bool intScale) -> VideoSize {
  uint w = 224;
  uint h = 224;
  double m;
  if(intScale) m = min(width / w, height / h);
  else         m = min(width / (double)w, height / (double)h);
  return {(uint)(w * m), (uint)(h * m)};
}

auto Interface::videoFrequency() -> double {
  return 3'072'000.0 / (159.0 * 256.0);  //~75.47hz
}

auto Interface::audioFrequency() -> double {
  return 3'072'000.0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.information.sha256;
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  if(port == 0 && system.orientation() != device) system.rotate();
}

auto Interface::power() -> void {
  system.power();
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::rotate() -> void {
  system.rotate();
  platform->deviceChanged(0, system.orientation());
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

}
