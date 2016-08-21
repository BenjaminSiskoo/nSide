#include <ms/ms.hpp>

namespace MasterSystem {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  system.init();

  information.preAlpha     = true;
  information.manufacturer = "Sega";
  information.name         = "Master System";
  information.overscan     = true;
  information.resettable   = true;

  information.capability.states = false;
  information.capability.cheats = false;

  media.append({ID::SG1000,       "SG-1000",       "sg"});
  media.append({ID::MasterSystem, "Master System", "ms"});
  media.append({ID::GameGear,     "Game Gear",     "gg"});

  Port controllerPort1{ID::Port::Controller1, "Controller Port 1", PlugAndPlay};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2", PlugAndPlay};

  { Device device{ID::Device::None, "None"};
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Gamepad, "Gamepad"};
    device.inputs.append({0, "Up"});
    device.inputs.append({0, "Down"});
    device.inputs.append({0, "Left"});
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "1"});
    device.inputs.append({0, "2"});
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoSize() -> VideoSize {
  switch(system.model()) {
  case Model::SG1000:       return {256, 192};
  case Model::MasterSystem: return {256, 240};
  case Model::GameGear:     return {160, 144};
  }
  unreachable;
}

auto Interface::videoSize(uint width, uint height, bool arc) -> VideoSize {
  double w = system.model() != Model::GameGear ? 256 : 160;
  if(arc && system.model() != Model::GameGear) {
    double squarePixelRate = system.region() == System::Region::NTSC
    ? 135.0 / 22.0 * 1'000'000.0
    : 7'375'000.0;
    w *= squarePixelRate / (system.colorburst() * 6.0 / (system.region() == System::Region::NTSC ? 4.0 : 4.0));
  }
  uint h = system.model() == Model::SG1000 ? 192 : system.model() == Model::MasterSystem ? 240 : 144;
  uint m = min((uint)(width / w), height / h);
  return {(uint)(w * m), h * m};
}

auto Interface::videoFrequency() -> double {
  return 60.0;
}

auto Interface::videoColors() -> uint32 {
  switch(system.model()) {
  case Model::SG1000:       return 1 <<  4;
  case Model::MasterSystem: return 1 <<  6;
  case Model::GameGear:     return 1 << 12;
  }
  unreachable;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint64 r;
  uint64 g;
  uint64 b;

  if(system.model() == Model::SG1000) {
    double gamma = settings.colorEmulation ? 1.8 : 2.2;

    static double Y[] = {
      0.00, 0.00, 0.53, 0.67,
      0.40, 0.53, 0.47, 0.67,
      0.53, 0.67, 0.73, 0.80,
      0.46, 0.53, 0.80, 1.00,
    };
    static double Saturation[] = {
      0.000, 0.000, 0.267, 0.200,
      0.300, 0.267, 0.233, 0.300,
      0.300, 0.300, 0.233, 0.167,
      0.233, 0.200, 0.000, 0.000,
    };
    static uint Phase[] = {
        0,   0, 204, 202,
      321, 321,  81, 262,
       81,  81, 140, 140,
      202,  20,   0,   0,
    };
    double y = Y[color];
    double i = Saturation[color] * std::sin(Phase[color] * Math::Pi / 180.0);
    double q = Saturation[color] * std::cos(Phase[color] * Math::Pi / 180.0);

    auto gammaAdjust = [=](double f) -> double { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    //This matrix is from FCC's 1953 NTSC standard.
    //The SG-1000, ColecoVision, and MSX are older than the SMPTE C standard that followed in 1987.
    r = uclamp<16>(65535.0 * gammaAdjust(y +  0.946882 * i +  0.623557 * q));
    g = uclamp<16>(65535.0 * gammaAdjust(y + -0.274788 * i + -0.635691 * q));
    b = uclamp<16>(65535.0 * gammaAdjust(y + -1.108545 * i +  1.709007 * q));
  }

  if(system.model() == Model::MasterSystem) {
    uint R = color.bits(0,1);
    uint G = color.bits(2,3);
    uint B = color.bits(4,5);

    r = image::normalize(R, 2, 16);
    g = image::normalize(G, 2, 16);
    b = image::normalize(B, 2, 16);
  }

  if(system.model() == Model::GameGear) {
    uint R = color.bits( 0, 3);
    uint G = color.bits( 4, 7);
    uint B = color.bits( 8,11);

    r = image::normalize(R, 4, 16);
    g = image::normalize(G, 4, 16);
    b = image::normalize(B, 4, 16);
  }

  return r << 32 | g << 16 | b << 0;
}

auto Interface::audioFrequency() -> double {
  return 44'100.0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::load(uint id) -> bool {
  if(id == ID::SG1000) return system.load(Model::SG1000);
  if(id == ID::MasterSystem) return system.load(Model::MasterSystem);
  if(id == ID::GameGear) return system.load(Model::GameGear);
  return false;
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  system.unload();
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
