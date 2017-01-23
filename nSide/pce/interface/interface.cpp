#include <pce/pce.hpp>

namespace PCEngine {

Model model;
Settings settings;
#include "pc-engine.cpp"
#include "supergrafx.cpp"

Interface::Interface() {
  information.overscan = true;

  information.capability.states = false;
  information.capability.cheats = false;

  Port controllerPort{ID::Port::Controller, "Controller Port"};

  { Device device{ID::Device::None, "None"};
    controllerPort.devices.append(device);
  }

  { Device device{ID::Device::Gamepad, "Gamepad"};
    device.inputs.append({0, "Up"});
    device.inputs.append({0, "Down"});
    device.inputs.append({0, "Left"});
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "II"});
    device.inputs.append({0, "I"});
    device.inputs.append({0, "Select"});
    device.inputs.append({0, "Run"});
    controllerPort.devices.append(device);
  }

  ports.append(move(controllerPort));
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoSize() -> VideoSize {
  return {1140, 242};
}

auto Interface::videoSize(uint width, uint height, bool arc, bool intScale) -> VideoSize {
  double w = 285;
  if(arc) {
    double squarePixelRate = 135.0 / 22.0 * 1'000'000.0;
    w = round(w * (squarePixelRate / (system.colorburst() * 6.0 / 4.0)));
  }
  double h = 242;
  double m = min(width / w, height / h);
  if(intScale) m = floor(m);
  return {(uint)(w * m), (uint)(h * m)};
}

auto Interface::videoFrequency() -> double {
  return (system.colorburst() * 6.0) / (262.0 * 1365.0);
}

auto Interface::videoColors() -> uint32 {
  return 1 << 9;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint3 B = color.bits(0,2);
  uint3 R = color.bits(3,5);
  uint3 G = color.bits(6,8);

  uint64 r = image::normalize(R, 3, 16);
  uint64 g = image::normalize(G, 3, 16);
  uint64 b = image::normalize(B, 3, 16);

  return r << 32 | g << 16 | b << 0;
}

auto Interface::audioFrequency() -> double {
  return 32'087.5;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  PCEngine::peripherals.connect(port, device);
}

auto Interface::power() -> void {
  system.power();
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::serialize() -> serializer {
  return {};
}

auto Interface::unserialize(serializer& s) -> bool {
  return false;
}

auto Interface::cap(const string& name) -> bool {
  return false;
}

auto Interface::get(const string& name) -> any {
  return {};
}

auto Interface::set(const string& name, const any& value) -> bool {
  return false;
}

}
