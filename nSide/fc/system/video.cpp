auto System::configureVideo() -> void {
  Emulator::video.reset();
  Emulator::video.setInterface(interface);
  configureVideoPalette();
  configureVideoEffects();
  configureVideoCursors();
}

auto System::configureVideoPalette() -> void {
  static auto generateYIQColor = [](uint9 n, double saturation, double hue, double contrast, double brightness, double gamma) -> uint64 {
    int color = (n & 0x0f), level = color < 0xe ? (n >> 4) & 3 : 1;

    static const double black = 0.518, white = 1.962, attenuation = 0.746;
    static const double levels[8] = {
      0.350, 0.518, 0.962, 1.550,
      1.094, 1.506, 1.962, 1.962,
    };

    double lo_and_hi[2] = {
      levels[level + 4 * (color == 0x0)],
      levels[level + 4 * (color <  0xd)],
    };

    double y = 0.0, i = 0.0, q = 0.0;
    auto wave = [](int p, int color) { return (color + p + 8) % 12 < 6; };
    for(int p : range(12)) {
      double spot = lo_and_hi[wave(p, color)];

      if(((n & 0x040) && wave(p, 12))
      || ((n & 0x080) && wave(p,  4))
      || ((n & 0x100) && wave(p,  8))
      ) spot *= attenuation;

      double v = (spot - black) / (white - black);

      v = (v - 0.5) * contrast + 0.5;
      v *= brightness / 12.0;

      y += v;
      i += v * std::cos((3.141592653 / 6.0) * (p + hue));
      q += v * std::sin((3.141592653 / 6.0) * (p + hue));
    }

    i *= saturation;
    q *= saturation;

    auto gammaAdjust = [=](double f) { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    uint64 r = uclamp<16>(65535.0 * gammaAdjust(y +  0.946882 * i +  0.623557 * q));
    uint64 g = uclamp<16>(65535.0 * gammaAdjust(y + -0.274788 * i + -0.635691 * q));
    uint64 b = uclamp<16>(65535.0 * gammaAdjust(y + -1.108545 * i +  1.709007 * q));

    return r << 32 | g << 16 | b << 0;
  };

  static auto generateRGBColor = [](uint9 color, const uint9* palette) -> uint64 {
    uint3 r = color.bit(6) ? 7 : palette[color.bits(5,0)] >> 6 & 7;
    uint3 g = color.bit(7) ? 7 : palette[color.bits(5,0)] >> 3 & 7;
    uint3 b = color.bit(8) ? 7 : palette[color.bits(5,0)] >> 0 & 7;

    uint64 R = image::normalize(r, 3, 16);
    uint64 G = image::normalize(g, 3, 16);
    uint64 B = image::normalize(b, 3, 16);

    if(settings.colorEmulation) {
      //TODO: check how arcade displays alter the signal
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

  Emulator::video.setPalette(((1 << 9) << vs()) + pc10() * (1 << 8), [&](uint32 color) -> uint64 {
    bool rgb;
    const uint9* palette = nullptr;
    switch(ppu.revision) {
    case PPU::Revision::RP2C02C:
    case PPU::Revision::RP2C02G:
    case PPU::Revision::RP2C07:
      rgb = false;
      break;
    case PPU::Revision::RP2C03B:
    case PPU::Revision::RP2C03G:
    case PPU::Revision::RC2C03B:
    case PPU::Revision::RC2C03C:
    case PPU::Revision::RC2C05_01:
    case PPU::Revision::RC2C05_02:
    case PPU::Revision::RC2C05_03:
    case PPU::Revision::RC2C05_04:
    case PPU::Revision::RC2C05_05:
      rgb = true;
      palette = PPU::RP2C03;
      break;
    case PPU::Revision::RP2C04_0001:
      rgb = true;
      palette = PPU::RP2C04_0001;
      break;
    case PPU::Revision::RP2C04_0002:
      rgb = true;
      palette = PPU::RP2C04_0002;
      break;
    case PPU::Revision::RP2C04_0003:
      rgb = true;
      palette = PPU::RP2C04_0003;
      break;
    case PPU::Revision::RP2C04_0004:
      rgb = true;
      palette = PPU::RP2C04_0004;
      break;
    }

    if(!pc10() || color < (1 << 9)) {
      if(!rgb) {
        auto gamma = settings.colorEmulation ? 1.8 : 2.2;
        return generateYIQColor(color & 0x1ff, 2.0, 0.0, 1.0, 1.0, gamma);
      } else {
        return generateRGBColor(color & 0x1ff, palette);
      }
    } else {
      return generatePC10Color(color - (1 << 9));
    }
  });
}

auto System::configureVideoEffects() -> void {
  Emulator::video.setEffect(Emulator::Video::Effect::Scanlines, settings.scanlineEmulation);
  Emulator::video.resize(interface->information.canvasWidth, interface->information.canvasHeight);
}

auto System::configureVideoCursors() -> void {
  Emulator::video.clearCursors();
  if(settings.expansionPort == Device::BeamGun) { //Beam Gun
    if(dynamic_cast<BeamGun*>(peripherals.expansionPort)) {
      auto& controller = (BeamGun&)*peripherals.expansionPort;
      Emulator::video.addCursor(controller.cursor);
    }
  }
  if(settings.controllerPort2 == Device::BeamGun) { //Zapper
    if(dynamic_cast<BeamGun*>(peripherals.controllerPort2)) {
      auto& controller = (BeamGun&)*peripherals.controllerPort2;
      Emulator::video.addCursor(controller.cursor);
    }
  }
}
