VSSystemInterface::VSSystemInterface() {
  information.devState     = DevState::Full;
  information.manufacturer = "Nintendo";
  information.name         = "VS. System";

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
  switch((n < (1 << 9) ? ppu0 : ppu1).version) {
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
  return palette ? generateRGBColor(n & 0x1ff, palette) : (uint64)0ull;
}

auto VSSystemInterface::audioFrequency() -> double {
  return (system.colorburst() * 6.0) / 12.0;
}

auto VSSystemInterface::load(uint id) -> bool {
  return system.load(this, System::Model::VSSystem);
}

auto VSSystemInterface::connect(uint port, uint device) -> void {
}
