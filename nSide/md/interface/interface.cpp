#include <md/md.hpp>

namespace MegaDrive {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  system.init();

  information.preAlpha     = true;
  information.manufacturer = "Sega";
  information.name         = "Mega Drive";
  information.overscan     = true;
  information.resettable   = true;

  information.capability.states = false;
  information.capability.cheats = false;

  media.append({ID::MegaDrive, "Mega Drive", "md", Domain::Home});

  Port controllerPort1{ID::Port::Controller1, "Controller Port 1", PlugAndPlay};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2", PlugAndPlay};
  Port extensionPort{ID::Port::Extension, "Extension Port", PlugAndPlay};

  { Device device{ID::Device::None, "None"};
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
    extensionPort.devices.append(device);
  }

  { Device device{ID::Device::ControlPad, "Control Pad"};
    device.inputs.append({0, "Up"   });
    device.inputs.append({0, "Down" });
    device.inputs.append({0, "Left" });
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "A"    });
    device.inputs.append({0, "B"    });
    device.inputs.append({0, "C"    });
    device.inputs.append({0, "Start"});
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::FightingPad6B, "Fighting Pad 6B"};
    device.inputs.append({0, "Up"   });
    device.inputs.append({0, "Down" });
    device.inputs.append({0, "Left" });
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "A"    });
    device.inputs.append({0, "B"    });
    device.inputs.append({0, "C"    });
    device.inputs.append({0, "X"    });
    device.inputs.append({0, "Y"    });
    device.inputs.append({0, "Z"    });
    device.inputs.append({0, "Start"});
    device.inputs.append({0, "Mode" });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
  ports.append(move(extensionPort));
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoSize() -> VideoSize {
  return {1280, 480};
}

auto Interface::videoSize(uint width, uint height, bool arc) -> VideoSize {
  double w = 320;
  if(arc) {
    double squarePixelRate = 135.0 / 22.0 * 1'000'000.0;
    w *= squarePixelRate / (system.colorburst() * 15.0 / 8.0);
  }
  uint h = 240;
  uint m = min((uint)(width / w), height / h);
  return {(uint)(w * m), h * m};
}

auto Interface::videoFrequency() -> double {
  return 60.0;
}

auto Interface::videoColors() -> uint32 {
  return (1 << 9) * 3;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint R = color.bits(0,2);
  uint G = color.bits(3,5);
  uint B = color.bits(6,8);
  uint intensity = color.bits(9,10);

  switch(intensity) {
  case 0:  //shadow
    break;
  case 1:  //normal
    R <<= 1;
    G <<= 1;
    B <<= 1;
    break;
  case 2:  //highlight
    R += 7;
    G += 7;
    B += 7;
    break;
  }

  uint64 r = image::normalize(R, 4, 16) + image::normalize(R, 4, 16) / 14;
  uint64 g = image::normalize(G, 4, 16) + image::normalize(G, 4, 16) / 14;
  uint64 b = image::normalize(B, 4, 16) + image::normalize(B, 4, 16) / 14;

  return r << 32 | g << 16 | b << 0;
}

auto Interface::audioFrequency() -> double {
  return 52'000.0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::load(uint id) -> bool {
  return system.load();
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  MegaDrive::peripherals.connect(port, device);
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
