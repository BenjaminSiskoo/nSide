#include <a2600/a2600.hpp>

namespace Atari2600 {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  system.init();

  information.preAlpha     = true;
  information.manufacturer = "Atari";
  information.name         = "Atari 2600";
  information.width        = 160;
  information.height       = 228;
  information.overscan     = true;
  information.aspectRatio  = 12.0 / 7.0;
  information.resettable   = false;

  information.capability.states = true;
  information.capability.cheats = false;

  media.append({ID::Atari2600, "Atari 2600", "a26"});

  Port hardwarePort{ID::Port::Hardware, "Hardware", false};
  Port controllerPort1{ID::Port::Controller1, "Controller Port 1", true};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2", true};

  { Device device{ID::Device::Controls, "Controls"};
    device.inputs.append({0, "Select"          });
    device.inputs.append({0, "Reset"           });
    device.inputs.append({0, "Color"           });
    device.inputs.append({0, "Left Difficulty" });
    device.inputs.append({0, "Right Difficulty"});
    hardwarePort.devices.append(device);
  }

  { Device device{ID::Device::None, "None"};
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Joystick, "Joystick"};
    device.inputs.append({0, "Up"   });
    device.inputs.append({0, "Down" });
    device.inputs.append({0, "Left" });
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "Fire" });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  ports.append(move(hardwarePort));
  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC:  return system.colorburst() / (262.0 * 228.0);
  case System::Region::PAL:   return system.colorburst() / (312.0 * 228.0);
  case System::Region::SECAM: return system.colorburst() / (312.0 * 228.0);
  }
}

auto Interface::videoColors() -> uint32 {
  return 128;
}

auto Interface::videoColor(uint32 n) -> uint64 {
  static auto generateNTSCColor = [](uint7 n, double hue, double gamma) -> uint64 {
    uint4 color = n.bits(3,6);
    uint3 level = n.bits(0,2);

    double y;
    double i;
    double q;

    //TODO: Determine the real formula for generating colors. The below formula
    //is a quick hack-up match colors with publicly-available palettes.
    if(color == 0 && level == 0) y = 0.0;
    else y = 0.125 + level / 7.0 * 0.875;

    if(color == 0) {
      i = 0.0;
      q = 0.0;
    } else {
      double phase = Math::Pi + hue - (24.0 + color * 27.0809523) * Math::Pi / 180.0;
      i = std::sin(phase) * 0.1986;
      q = std::cos(phase) * 0.1742;
    }

    auto gammaAdjust = [=](double f) -> double { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    //This matrix is from FCC's 1953 NTSC standard.
    //The Atari 2600 is older than the SMPTE C standard that followed in 1987.
    uint64 r = uclamp<16>(65535.0 * gammaAdjust(y +  0.946882 * i +  0.623557 * q));
    uint64 g = uclamp<16>(65535.0 * gammaAdjust(y + -0.274788 * i + -0.635691 * q));
    uint64 b = uclamp<16>(65535.0 * gammaAdjust(y + -1.108545 * i +  1.709007 * q));

    return r << 32 | g << 16 | b << 0;
  };

  static auto generatePALColor = [](uint7 n, double gamma) -> uint64 {
    uint4 color = n.bits(3,6);
    uint3 level = n.bits(0,2);

    double y;
    double u;
    double v;

    //TODO: Determine the real formula for generating colors. The below formula
    //is a quick hack-up match colors with publicly-available palettes.
    if((color.bits(1,3) == 0 || color.bits(1,3) == 7) && level == 0) y = 0.0;
    else y = 0.125 + level / 7.0 * 0.875;

    if(color.bits(1,3) == 0 || color.bits(1,3) == 7) {
      u = 0.0;
      v = 0.0;
    } else if(color.bit(0) == 0) {
      double phase = ((n >> 5) * 30.0 - 90.0) * Math::Pi / 180.0;
      u = std::sin(phase) * 0.25;
      v = std::cos(phase) * 0.25;
    } else if(color.bit(0) == 1) {
      double phase = (285.0 - (n >> 5) * 30.0) * Math::Pi / 180.0;
      u = std::sin(phase) * 0.25;
      v = std::cos(phase) * 0.25;
    }

    auto gammaAdjust = [=](double f) -> double { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    uint64 r = uclamp<16>(65535.0 * gammaAdjust(y                 +  1.139837 * v));
    uint64 g = uclamp<16>(65535.0 * gammaAdjust(y + -0.394652 * u + -0.580599 * v));
    uint64 b = uclamp<16>(65535.0 * gammaAdjust(y +  2.032110 * u                ));

    return r << 32 | g << 16 | b << 0;
  };

  static auto generateSECAMColor = [](uint7 n) -> uint64 {
    uint3 level = n.bits(0,2);

    //TODO: Determine if these colors are generated through some method
    //other than RGB, and use that color space instead.
    static uint32 colors[] = {
      0xff000000, 0xff2121ff, 0xfff03c79, 0xffff50ff,
      0xff7fff50, 0xff7fffff, 0xffffff3f, 0xffffffff,
    };

    uint64 r = image::normalize(colors[level].byte(2), 8, 16);
    uint64 g = image::normalize(colors[level].byte(1), 8, 16);
    uint64 b = image::normalize(colors[level].byte(0), 8, 16);

    return r << 32 | g << 16 | b << 0;
  };

  double gamma = settings.colorEmulation ? 1.8 : 2.2;
  if(system.region() == System::Region::NTSC) {
    return generateNTSCColor(n, 0.0, gamma);
  } else if(system.region() == System::Region::PAL) {
    return generatePALColor(n, gamma);
  } else if(system.region() == System::Region::SECAM) {
    return generateSECAMColor(n);
  }
}

auto Interface::audioFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC:  return system.colorburst();
  case System::Region::PAL:   return system.colorburst();
  case System::Region::SECAM: return system.colorburst();
  }
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::load(uint id) -> bool {
  return system.load();
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  Atari2600::peripherals.connect(port, device);
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

auto Interface::serialize() -> serializer {
  system.runToSave();
  return system.serialize();
}

auto Interface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto Interface::cheatSet(const string_vector& list) -> void {
  cheat.reset();
  cheat.assign(list);
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
  if(name == "Color Emulation" && value.is<bool>()) {
    settings.colorEmulation = value.get<bool>();
    system.configureVideoPalette();
    return true;
  }
  if(name == "Scanline Emulation" && value.is<bool>()) {
    settings.scanlineEmulation = value.get<bool>();
    system.configureVideoEffects();
    return true;
  }
  return false;
}

}
