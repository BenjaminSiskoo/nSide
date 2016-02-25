Video video;

Video::Video() {
  // * 2 for scanline emulation, * 2 again for VS. System and PlayChoice-10 extra screens
  output = new uint32[256 * 240 * 2 * 2];

  // * 2 for second VS. System PPU (of which 256 colors are used for PlayChoice-10)
  paletteLiteral = new uint32[(1 << 9) * 2];
  paletteStandard = new uint32[(1 << 9) * 2];
  paletteEmulation = new uint32[(1 << 9) * 2];
}

auto Video::reset() -> void {
  memory::fill(output(), 256 * 240 * 2 * 2 * sizeof(uint32));

  bool rgb;
  const uint9* ppu_pal = nullptr;
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
    ppu_pal = PPU::RP2C03;
    break;
  case PPU::Revision::RP2C04_0001:
    rgb = true;
    ppu_pal = PPU::RP2C04_0001;
    break;
  case PPU::Revision::RP2C04_0002:
    rgb = true;
    ppu_pal = PPU::RP2C04_0002;
    break;
  case PPU::Revision::RP2C04_0003:
    rgb = true;
    ppu_pal = PPU::RP2C04_0003;
    break;
  case PPU::Revision::RP2C04_0004:
    rgb = true;
    ppu_pal = PPU::RP2C04_0004;
    break;
  }

  if(!rgb) {
    for(auto color : range((1 << 9) * 2)) {
      paletteLiteral[color] = color;
      paletteStandard[color] = generateColor(color, 2.0, 0.0, 1.0, 1.0, 2.2);
      paletteEmulation[color] = generateColor(color, 2.0, 0.0, 1.0, 1.0, 1.8);
    }
  } else {
    generatePalettes(ppu_pal);
  }
}

auto Video::refresh() -> void {
  if(system.vs() && interface->information.width == 512) {
    refreshVSDualSystem();
  } else if(system.pc10()) {
    if(interface->information.height == 480) {
      refreshPC10DualScreen();
    } else if(pc10arcadeboard.display == 0) {
      refreshPC10Menu();
    } else {
      refreshMain();
    }
  } else {
    refreshMain();
  }
}

auto Video::refreshMain() -> void {
  auto output = this->output();
  auto& palette = settings.colorEmulation ? paletteEmulation : paletteStandard;

  if(settings.scanlineEmulation) {
    for(uint y = 0; y < 240; y++) {
      auto source = ppu.output + y * 256;
      auto targetLo = output + y * 512;
      auto targetHi = output + y * 512 + 256;
      for(uint x = 0; x < 256; x++) {
        auto color = palette[*source++];
        *targetLo++ = color;
        *targetHi++ = (255 << 24) | ((color & 0xfefefe) >> 1);
      }
    }
  } else {
    for(uint y = 0; y < 240; y++) {
      auto source = ppu.output + y * 256;
      auto target = output + y * 256;
      for(uint x = 0; x < 256; x++) {
        *target++ = palette[*source++];
      }
    }
  }

  drawCursors();
  interface->videoRefresh(output, 256 * sizeof(uint32), 256, settings.scanlineEmulation ? 480 : 240);
}

auto Video::refreshVSDualSystem() -> void {
  auto output = this->output();
  auto& palette = settings.colorEmulation ? paletteEmulation : paletteStandard;

  if(settings.scanlineEmulation) {
    for(uint y = 0; y < 240; y++) {
      auto sourceMain = ppu.output + y * 256;
      auto sourceSub = ppu.output + y * 256;
      auto targetMainLo = output + y * 1024;
      auto targetMainHi = output + y * 1024 + 512;
      auto targetSubLo = output + y * 1024 + 256;
      auto targetSubHi = output + y * 1024 + 256 + 512;
      for(uint x = 0; x < 256; x++) {
        auto colorMain = palette[*sourceMain++];
        auto colorSub = palette[*sourceSub++];
        *targetMainLo++ = colorMain;
        *targetMainHi++ = (255 << 24) | ((colorMain & 0xfefefe) >> 1);
        *targetSubLo++ = colorSub;
        *targetSubHi++ = (255 << 24) | ((colorSub & 0xfefefe) >> 1);
      }
    }
  } else {
    for(uint y = 0; y < 240; y++) {
      auto sourceMain = ppu.output + y * 256;
      auto sourceSub = ppu.output + y * 256;
      auto targetMain = output + y * 512;
      auto targetSub = output + y * 512 + 256;
      for(uint x = 0; x < 256; x++) {
        *targetMain++ = palette[*sourceMain++];
        *targetSub++ = palette[*sourceSub++];
      }
    }
  }

  drawCursors();
  interface->videoRefresh(output, 512 * sizeof(uint32), 512, settings.scanlineEmulation ? 480 : 240);
}

auto Video::refreshPC10DualScreen() -> void {
  auto output = this->output();
  auto& palette = settings.colorEmulation ? paletteEmulation : paletteStandard;

  if(settings.scanlineEmulation) {
    for(uint y = 0; y < 240; y++) {
      auto sourceGame = ppu.output + y * 256;
      auto sourceMenu = pc10arcadeboard.video_output + y * 256;
      auto targetGameLo = output + y * 512 + 512 * 240;
      auto targetGameHi = output + y * 512 + 512 * 240 + 256;
      auto targetMenuLo = output + y * 512 + 512 *   0;
      auto targetMenuHi = output + y * 512 + 512 *   0 + 256;
      for(uint x = 0; x < 256; x++) {
        auto colorGame = palette[*sourceGame++];
        auto colorMenu = palette[(1 << 9) + *sourceMenu++];
        *targetGameLo++ = colorGame;
        *targetGameHi++ = (255 << 24) | ((colorGame & 0xfefefe) >> 1);
        *targetMenuLo++ = colorMenu;
        *targetMenuHi++ = (255 << 24) | ((colorMenu & 0xfefefe) >> 1);
      }
    }
  } else {
    for(uint y = 0; y < 240; y++) {
      auto sourceGame = ppu.output + y * 256;
      auto sourceMenu = pc10arcadeboard.video_output + y * 256;
      auto targetGame = output + y * 256 + 256 * 240;
      auto targetMenu = output + y * 256;
      for(uint x = 0; x < 256; x++) {
        *targetGame++ = palette[*sourceGame++];
        *targetMenu++ = palette[(1 << 9) + *sourceMenu++];
      }
    }
  }

  drawCursors();
  interface->videoRefresh(output, 256 * sizeof(uint32), 256, settings.scanlineEmulation ? 960 : 480);
}

auto Video::refreshPC10Menu() -> void {
  auto output = this->output();
  auto& palette = settings.colorEmulation ? paletteEmulation : paletteStandard;

  if(settings.scanlineEmulation) {
    for(uint y = 0; y < 240; y++) {
      auto source = pc10arcadeboard.video_output + y * 256;
      auto targetLo = output + y * 512;
      auto targetHi = output + y * 512 + 256;
      for(uint x = 0; x < 256; x++) {
        auto color = palette[(1 << 9) + *source++];
        *targetLo++ = color;
        *targetHi++ = (255 << 24) | ((color & 0xfefefe) >> 1);
      }
    }
  } else {
    for(uint y = 0; y < 240; y++) {
      auto source = pc10arcadeboard.video_output + y * 256;
      auto target = output + y * 256;
      for(uint x = 0; x < 256; x++) {
        *target++ = palette[(1 << 9) + *source++];
      }
    }
  }

  interface->videoRefresh(output, 256 * sizeof(uint32), 256, settings.scanlineEmulation ? 480 : 240);
}

auto Video::generateColor(
  uint n, double saturation, double hue,
  double contrast, double brightness, double gamma
) -> uint32 {
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
  uint r = uclamp<16>(65535.0 * gammaAdjust(y +  0.946882 * i +  0.623557 * q));
  uint g = uclamp<16>(65535.0 * gammaAdjust(y + -0.274788 * i + -0.635691 * q));
  uint b = uclamp<16>(65535.0 * gammaAdjust(y + -1.108545 * i +  1.709007 * q));

  return interface->videoColor(r, g, b);
}

auto Video::generatePalettes(const uint9* ppu_pal) -> void {
  for(uint color = 0; color < (1 << 9); color++) {
    //if(mode == Emulator::Interface::PaletteMode::Literal) {
    //  palette[color] = color;
    //  continue;
    //}
    uint r = (color & 0x040) ? 7 : ((ppu_pal[color & 0x3f] >> 6) & 7);
    uint g = (color & 0x080) ? 7 : ((ppu_pal[color & 0x3f] >> 3) & 7);
    uint b = (color & 0x100) ? 7 : ((ppu_pal[color & 0x3f] >> 0) & 7);

    paletteLiteral[color] = color;

    uint R = image::normalize(r, 3, 16);
    uint G = image::normalize(g, 3, 16);
    uint B = image::normalize(b, 3, 16);
    paletteStandard[color] = interface->videoColor(R, G, B);

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
    paletteEmulation[color] = interface->videoColor(R, G, B);
  }
  if(system.pc10()) {
    for(uint color = 0; color < 256; color++) {
      uint r = 15 - pc10arcadeboard.cgrom[color + 0x000];
      uint g = 15 - pc10arcadeboard.cgrom[color + 0x100];
      uint b = 15 - pc10arcadeboard.cgrom[color + 0x200];

      uint R = image::normalize(r, 4, 16);
      uint G = image::normalize(g, 4, 16);
      uint B = image::normalize(b, 4, 16);
      paletteStandard[(1 << 9) + color] = interface->videoColor(R, G, B);
      
      //TODO: check the menu monitor's gamma ramp
      R = image::normalize(r, 4, 16);
      G = image::normalize(g, 4, 16);
      B = image::normalize(b, 4, 16);
      paletteEmulation[(1 << 9) + color] = interface->videoColor(R, G, B);
    }
    /*
    } else if(mode == Emulator::Interface::PaletteMode::Channel) {
      for(uint color = 0; color < 256; color++) {
        palette[(1 << 9) + color] = interface->videoColor((1 << 9) + color,
          65535, // distinguish from the game screen
          image::normalize(15 - pc10arcadeboard.cgrom[color + 0x000], 4, 16),
          image::normalize(15 - pc10arcadeboard.cgrom[color + 0x100], 4, 16),
          image::normalize(15 - pc10arcadeboard.cgrom[color + 0x200], 4, 16)
        );
      }
    }
    */
  }
}

auto Video::drawCursor(uint32 color, int x, int y) -> void {
  static const uint8 cursor[15 * 15] = {
    0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
    0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,
    0,0,0,1,2,2,1,2,1,2,2,1,0,0,0,
    0,0,1,2,1,1,0,1,0,1,1,2,1,0,0,
    0,1,2,1,0,0,0,1,0,0,0,1,2,1,0,
    0,1,2,1,0,0,1,2,1,0,0,1,2,1,0,
    1,2,1,0,0,1,1,2,1,1,0,0,1,2,1,
    1,2,2,1,1,2,2,2,2,2,1,1,2,2,1,
    1,2,1,0,0,1,1,2,1,1,0,0,1,2,1,
    0,1,2,1,0,0,1,2,1,0,0,1,2,1,0,
    0,1,2,1,0,0,0,1,0,0,0,1,2,1,0,
    0,0,1,2,1,1,0,1,0,1,1,2,1,0,0,
    0,0,0,1,2,2,1,2,1,2,2,1,0,0,0,
    0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,
    0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
  };

  auto output = this->output();
  for(int cy = 0; cy < 15; cy++) {
    int vy = y + cy - 7;
    if(vy <= 0 || vy >= 240) continue;  //do not draw offscreen

    for(int cx = 0; cx < 15; cx++) {
      int vx = x + cx - 7;
      if(vx < 0 || vx >= 256) continue;  //do not draw offscreen
      uint8 pixel = cursor[cy * 15 + cx];
      if(pixel == 0) continue;
      uint32 pixelcolor = pixel == 1 ? (uint32)(255 << 24) : color;

      *(output + vy * 256 + vx) = pixelcolor;
    }
  }
}

auto Video::drawCursors() -> void {
  if((Device::ID)settings.controllerPort2 == Famicom::Device::ID::BeamGun) { // Zapper
    if(dynamic_cast<BeamGun*>(device.controllerPort2)) {
      auto& controller = (BeamGun&)*device.controllerPort2;
      drawCursor(0xfffd9b00, controller.x, controller.y);
    }
  }
  if((Device::ID)settings.expansionPort == Famicom::Device::ID::BeamGun) { // Beam Gun
    if(dynamic_cast<BeamGun*>(device.expansionPort)) {
      auto& controller = (BeamGun&)*device.expansionPort;
      drawCursor(0xff4e4e4e, controller.x, controller.y);
    }
  }
}
