PlayChoice10Interface::PlayChoice10Interface() {
  system.init();

  information.devState     = DevState::Full;
  information.manufacturer = "Nintendo";
  information.name         = "PlayChoice-10";
  information.overscan     = true;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::PlayChoice10, "PlayChoice-10", "pc10"});

  Port hardware{ID::Port::Hardware, "Hardware"};
  Port controllerPort1{ID::Port::Controller1, "Controller Port 1"};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2"};
  Port expansionPort{ID::Port::Expansion, "Expansion Port"};

  { Device device{ID::Device::PlayChoice10Controls, "PlayChoice-10 Controls"};
    device.inputs.append({0, "Game Select"   });
    device.inputs.append({0, "Start"         });
    device.inputs.append({0, "Channel Select"});
    device.inputs.append({0, "Enter"         });
    device.inputs.append({0, "Reset"         });
    device.inputs.append({0, "Service Button"});
    device.inputs.append({0, "Coin 1"        });
    device.inputs.append({0, "Coin 2"        });
    hardware.devices.append(device);
  }

  { Device device{ID::Device::None, "None"};
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::Gamepad, "Gamepad"};
    device.inputs.append({0, "Up"   });
    device.inputs.append({0, "Down" });
    device.inputs.append({0, "Left" });
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "B"    });
    device.inputs.append({0, "A"    });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::BeamGun, "Zapper"};
    device.inputs.append({1, "X-axis" });
    device.inputs.append({1, "Y-axis" });
    device.inputs.append({0, "Trigger"});
    expansionPort.devices.append(device);
  }

  ports.append(move(hardware));
  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
  ports.append(move(expansionPort));
}

auto PlayChoice10Interface::manifest() -> string {
  return cartridge.manifest();
}

auto PlayChoice10Interface::title() -> string {
  return cartridge.title();
}

auto PlayChoice10Interface::videoSize() -> VideoSize {
  return {256, 240 + (playchoice10.screenConfig - 1) * 224};
}

auto PlayChoice10Interface::videoSize(uint width, uint height, bool arc, bool intScale) -> VideoSize {
  double w = 256 / playchoice10.screenConfig;
  if(arc) {
    double squarePixelRate = 135.0 / 22.0 * 1'000'000.0;
    w *= squarePixelRate / (system.colorburst() * 6.0 / 4.0);
  }
  int h = playchoice10.screenConfig == 2 ? (240 + 224) / 2 : 240;
  double m;
  if(intScale) m = min((uint)(width / w), height / h);
  else         m = min(width / w, height / (double)h);
  return {(uint)(w * m), (uint)(h * m)};
}

auto PlayChoice10Interface::videoFrequency() -> double {
  return (system.colorburst() * 6.0) / (262.0 * 1364.0);
}

auto PlayChoice10Interface::videoColors() -> uint32 {
  return (1 << 9) + (1 << 8);
}

auto PlayChoice10Interface::videoColor(uint32 n) -> uint64 {
  static auto generateRGBColor = [](uint9 color, const uint9* palette) -> uint64 {
    uint3 r = color.bit(6) ? 7 : palette[color.bits(5,0)].bits(6,8);
    uint3 g = color.bit(7) ? 7 : palette[color.bits(5,0)].bits(3,5);
    uint3 b = color.bit(8) ? 7 : palette[color.bits(5,0)].bits(0,2);

    uint64 R = image::normalize(r, 3, 16);
    uint64 G = image::normalize(g, 3, 16);
    uint64 B = image::normalize(b, 3, 16);

    if(settings.colorEmulation) {
      //TODO: check how arcade displays alter the signal.
      //The red, green, and blue channels are connected directly without any
      //conversion to YIQ/YUV/YPbPr/etc. and back.
      static const uint8 gammaRamp[8] = {
        0x00, 0x0a,
        0x2d, 0x5b,
        0x98, 0xb8,
        0xe0, 0xff,
      };
      R = gammaRamp[r] * 0x0101;
      G = gammaRamp[g] * 0x0101;
      B = gammaRamp[b] * 0x0101;
    }

    return R << 32 | G << 16 | B << 0;
  };

  static auto generatePC10Color = [](uint9 color) -> uint64 {
    uint r = 15 - playchoice10.videoCircuit.cgrom[color + 0x000];
    uint g = 15 - playchoice10.videoCircuit.cgrom[color + 0x100];
    uint b = 15 - playchoice10.videoCircuit.cgrom[color + 0x200];

    uint64 R = image::normalize(r, 4, 16);
    uint64 G = image::normalize(g, 4, 16);
    uint64 B = image::normalize(b, 4, 16);

    if(settings.colorEmulation) {
      //TODO: check the menu monitor's gamma ramp
      static const uint8 gammaRamp[16] = {
        0x00, 0x03, 0x0a, 0x15,
        0x24, 0x37, 0x4e, 0x69,
        0x90, 0xa0, 0xb0, 0xc0,
        0xd0, 0xe0, 0xf0, 0xff,
      };
      R = gammaRamp[r] * 0x0101;
      G = gammaRamp[g] * 0x0101;
      B = gammaRamp[b] * 0x0101;
    }

    return R << 32 | G << 16 | B << 0;
  };

  if(n < (1 << 9)) {
    const uint9* palette = nullptr;
    switch(ppu.version) {
    case PPU::Version::RP2C03B:
    case PPU::Version::RP2C03G:
    case PPU::Version::RC2C03B:
    case PPU::Version::RC2C03C:
    case PPU::Version::RC2C05_01:
    case PPU::Version::RC2C05_02:
    case PPU::Version::RC2C05_03:
    case PPU::Version::RC2C05_04:
    case PPU::Version::RC2C05_05:
      palette = PPU::RP2C03;
      break;
    case PPU::Version::RP2C04_0001:
      palette = PPU::RP2C04_0001;
      break;
    case PPU::Version::RP2C04_0002:
      palette = PPU::RP2C04_0002;
      break;
    case PPU::Version::RP2C04_0003:
      palette = PPU::RP2C04_0003;
      break;
    case PPU::Version::RP2C04_0004:
      palette = PPU::RP2C04_0004;
      break;
    }
    return generateRGBColor(n & 0x1ff, palette);
  } else {
    return generatePC10Color(n - (1 << 9));
  }
}

auto PlayChoice10Interface::audioFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC:  return (system.colorburst() * 6.0) / 12.0;
  case System::Region::PAL:   return (system.colorburst() * 6.0) / 16.0;
  case System::Region::Dendy: return (system.colorburst() * 6.0) / 15.0;
  }
}

auto PlayChoice10Interface::loaded() -> bool {
  return system.loaded();
}

auto PlayChoice10Interface::sha256() -> string {
  return cartridge.sha256();
}

auto PlayChoice10Interface::load(uint id) -> bool {
  return system.load(this, Model::PlayChoice10);
}

auto PlayChoice10Interface::save() -> void {
  system.save();
}

auto PlayChoice10Interface::unload() -> void {
  save();
  system.unload();
}

auto PlayChoice10Interface::connect(uint port, uint device) -> void {
  peripherals.connect(port, device);
}

auto PlayChoice10Interface::power() -> void {
  system.power();
}

auto PlayChoice10Interface::run() -> void {
  system.run();
}

auto PlayChoice10Interface::serialize() -> serializer {
  system.runToSave();
  return system.serialize();
}

auto PlayChoice10Interface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto PlayChoice10Interface::cheatSet(const string_vector& list) -> void {
  cheat.reset();
  cheat.assign(list);
}

auto PlayChoice10Interface::cap(const string& name) -> bool {
  if(name == "Color Emulation") return true;
  if(name == "Scanline Emulation") return true;
  return false;
}

auto PlayChoice10Interface::get(const string& name) -> any {
  if(name == "Color Emulation") return settings.colorEmulation;
  if(name == "Scanline Emulation") return settings.scanlineEmulation;
  return {};
}

auto PlayChoice10Interface::set(const string& name, const any& value) -> bool {
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

auto PlayChoice10Interface::exportMemory() -> void {
  string pathname = {platform->path(cartridge.pathID()), "debug/"};
  directory::create(pathname);

  if(auto fp = platform->open(cartridge.pathID(), "debug/work.ram", File::Write)) fp->write(cpu.ram, 0x800);
  if(cartridge.board->prgram.size()) if(auto fp = platform->open(cartridge.pathID(), "debug/program.ram", File::Write)) {
    fp->write(cartridge.board->prgram.data(), cartridge.board->prgram.size());
  }
  if(cartridge.board->chrram.size()) if(auto fp = platform->open(cartridge.pathID(), "debug/character.ram", File::Write)) {
    fp->write(cartridge.board->chrram.data(), cartridge.board->chrram.size());
  }
  if(!cartridge.board->chip) return;
  if(cartridge.board->chip->ram.size()) if(auto fp = platform->open(cartridge.pathID(), "debug/chip.ram", File::Write)) {
    fp->write(cartridge.board->chip->ram.data(), cartridge.board->chip->ram.size());
  }
}
