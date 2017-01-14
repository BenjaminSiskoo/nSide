VSSystemInterface::VSSystemInterface() {
  system.init();

  information.preAlpha     = false;
  information.manufacturer = "Nintendo";
  information.name         = "VS. System";
  information.overscan     = true;
  information.resettable   = true;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::VSSystem, "VS. System", "vs"});

  Port hardware{ID::Port::Hardware, "Hardware"};
  Port controllerPort1{ID::Port::Controller1, "Controller Port 1"};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2"};

  { Device device{ID::Device::VSSystemControls, "VS. System Controls"};
    device.inputs.append({0, "Button 1"      });
    device.inputs.append({0, "Button 2"      });
    device.inputs.append({0, "Button 3"      });
    device.inputs.append({0, "Button 4"      });
    device.inputs.append({0, "Service Button"});
    device.inputs.append({0, "Coin 1"        });
    device.inputs.append({0, "Coin 2"        });
    hardware.devices.append(device);
  }

  { Device device{ID::Device::None, "None"};
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Gamepad, "Gamepad"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Zapper, "Zapper"};
    device.inputs.append({1, "X-axis" });
    device.inputs.append({1, "Y-axis" });
    device.inputs.append({0, "Trigger"});
    controllerPort2.devices.append(device);
  }

  ports.append(move(hardware));
  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
}

auto VSSystemInterface::manifest() -> string {
  return cartridge.manifest();
}

auto VSSystemInterface::title() -> string {
  return cartridge.title();
}

auto VSSystemInterface::videoSize() -> VideoSize {
  return {256 * vssystem.gameCount, 240};
}

auto VSSystemInterface::videoSize(uint width, uint height, bool arc, bool intScale) -> VideoSize {
  double w = 256;
  if(arc) {
    double squarePixelRate = 135.0 / 22.0 * 1'000'000.0;
    w *= squarePixelRate / (system.colorburst() * 6.0 / 4.0);
  }
  int h = 240 / vssystem.gameCount;
  double m;
  if(intScale) m = min((uint)(width / w), height / h);
  else         m = min(width / w, height / (double)h);
  return {(uint)(w * m), (uint)(h * m)};
}

auto VSSystemInterface::videoFrequency() -> double {
  return (system.colorburst() * 6.0) / (262.0 * 1364.0);
}

auto VSSystemInterface::videoColors() -> uint32 {
  return (1 << 9) << 1;
}

auto VSSystemInterface::videoColor(uint32 n) -> uint64 {
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
}

auto VSSystemInterface::audioFrequency() -> double {
  return (system.colorburst() * 6.0) / 12.0;
}

auto VSSystemInterface::loaded() -> bool {
  return system.loaded();
}

auto VSSystemInterface::sha256() -> string {
  return cartridge.sha256();
}

auto VSSystemInterface::load(uint id) -> bool {
  return system.load(this, Model::VSSystem);
}

auto VSSystemInterface::save() -> void {
  system.save();
}

auto VSSystemInterface::unload() -> void {
  save();
  system.unload();
}

auto VSSystemInterface::connect(uint port, uint device) -> void {
}

auto VSSystemInterface::power() -> void {
  system.power();
}

auto VSSystemInterface::reset() -> void {
  system.reset();
}

auto VSSystemInterface::run() -> void {
  system.run();
}

auto VSSystemInterface::serialize() -> serializer {
  system.runToSave();
  return system.serialize();
}

auto VSSystemInterface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto VSSystemInterface::cheatSet(const string_vector& list) -> void {
  cheat.reset();
  cheat.assign(list);
}

auto VSSystemInterface::cap(const string& name) -> bool {
  if(name == "Color Emulation") return true;
  if(name == "Scanline Emulation") return true;
  return false;
}

auto VSSystemInterface::get(const string& name) -> any {
  if(name == "Color Emulation") return settings.colorEmulation;
  if(name == "Scanline Emulation") return settings.scanlineEmulation;
  return {};
}

auto VSSystemInterface::set(const string& name, const any& value) -> bool {
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

auto VSSystemInterface::exportMemory() -> void {
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