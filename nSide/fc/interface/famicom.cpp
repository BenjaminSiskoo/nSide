FamicomInterface::FamicomInterface() {
  information.devState     = DevState::Full;
  information.manufacturer = "Nintendo";
  information.name         = "Famicom";

  media.append({ID::Famicom, "Famicom", "fc"});

  Port hardware{ID::Port::Hardware, "Hardware"};
  Port controllerPort1{ID::Port::Controller1, "Controller Port 1"};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2"};
  Port expansionPort{ID::Port::Expansion, "Expansion Port"};

  { Device device{ID::Device::FamicomControls, "Famicom Controls"};
    device.inputs.append({0, "Reset"});
    hardware.devices.append(device);
  }

  { Device device{ID::Device::None, "None"};
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::Gamepad, "Gamepad"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    device.inputs.append({0, "Select"});
    device.inputs.append({0, "Start" });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::GamepadMic, "Gamepad Mic"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    device.inputs.append({0, "Mic"   });
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::FourScore, "Four Score"};
    for(uint p : {1,3}) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({0, {"Port ", p, " - ", "Start" }});
    }
    controllerPort1.devices.append(device);
  }

  { Device device{ID::Device::FourScore, "Four Score"};
    for(uint p : {2,4}) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({0, {"Port ", p, " - ", "Start" }});
    }
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Zapper, "Zapper"};
    device.inputs.append({1, "X-axis" });
    device.inputs.append({1, "Y-axis" });
    device.inputs.append({0, "Trigger"});
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::PowerPad, "Power Pad"};
    for(uint n : range(12)) {
      device.inputs.append({0, {"Button ", n + 1}});
    }
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Vaus, "Arkanoid Vaus"};
    device.inputs.append({1, "Control Knob"});
    device.inputs.append({0, "Fire Button" });
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::SNESGamepad, "SNES Gamepad"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    device.inputs.append({0, "Y"     });
    device.inputs.append({0, "X"     });
    device.inputs.append({0, "L"     });
    device.inputs.append({0, "R"     });
    device.inputs.append({0, "Select"});
    device.inputs.append({0, "Start" });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Mouse, "Mouse"};
    device.inputs.append({1, "X-axis"});
    device.inputs.append({1, "Y-axis"});
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::GamepadE, "Gamepad"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    device.inputs.append({0, "Select"});
    device.inputs.append({0, "Start" });
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::JoyPair, "JoyPair"};
    for(uint p : {3,4}) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({0, {"Port ", p, " - ", "Start" }});
    }
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::FourPlayers, "4-Players Adaptor"};
    for(uint p : {1,2,3,4}) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({0, {"Port ", p, " - ", "Start" }});
    }
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::BeamGun, "Beam Gun"};
    device.inputs.append({1, "X-axis" });
    device.inputs.append({1, "Y-axis" });
    device.inputs.append({0, "Trigger"});
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::FamilyKeyboard, "Family BASIC Keyboard"};
    device.inputs.append({0, "F1"});
    device.inputs.append({0, "F2"});
    device.inputs.append({0, "F3"});
    device.inputs.append({0, "F4"});
    device.inputs.append({0, "F5"});
    device.inputs.append({0, "F6"});
    device.inputs.append({0, "F7"});
    device.inputs.append({0, "F8"});
    device.inputs.append({0, "1 - Exclamation Mark"});
    device.inputs.append({0, "2 - Quotation Mark"});
    device.inputs.append({0, "3 - Number Sign"});
    device.inputs.append({0, "4 - Dollar Sign"});
    device.inputs.append({0, "5 - Percent Sign"});
    device.inputs.append({0, "6 - Ampersand"});
    device.inputs.append({0, "7 - Apostrophe"});
    device.inputs.append({0, "8 - Left Parenthesis"});
    device.inputs.append({0, "9 - Right Parenthesis"});
    device.inputs.append({0, "0"});
    device.inputs.append({0, "Hyphen-Minus - Equals Sign"});
    device.inputs.append({0, "Circumflex Accent"});
    device.inputs.append({0, "En Sign"});
    device.inputs.append({0, "STOP"});
    device.inputs.append({0, "ESC"});
    device.inputs.append({0, "Q"});
    device.inputs.append({0, "W"});
    device.inputs.append({0, "E"});
    device.inputs.append({0, "R"});
    device.inputs.append({0, "T"});
    device.inputs.append({0, "Y"});
    device.inputs.append({0, "U"});
    device.inputs.append({0, "I"});
    device.inputs.append({0, "O"});
    device.inputs.append({0, "P"});
    device.inputs.append({0, "Commercial At"});
    device.inputs.append({0, "Left Square Bracket"});
    device.inputs.append({0, "RETURN"});
    device.inputs.append({0, "CTR"});
    device.inputs.append({0, "A"});
    device.inputs.append({0, "S"});
    device.inputs.append({0, "D"});
    device.inputs.append({0, "F"});
    device.inputs.append({0, "G"});
    device.inputs.append({0, "H"});
    device.inputs.append({0, "J"});
    device.inputs.append({0, "K"});
    device.inputs.append({0, "L"});
    device.inputs.append({0, "Semicolon - Plus Sign"});
    device.inputs.append({0, "Colon - Asterisk"});
    device.inputs.append({0, "Right Square Bracket"});
    device.inputs.append({0, "Kana"});
    device.inputs.append({0, "SHIFT Left"});
    device.inputs.append({0, "Z"});
    device.inputs.append({0, "X"});
    device.inputs.append({0, "C"});
    device.inputs.append({0, "V"});
    device.inputs.append({0, "B"});
    device.inputs.append({0, "N"});
    device.inputs.append({0, "M"});
    device.inputs.append({0, "Comma - Less-Than Sign"});
    device.inputs.append({0, "Full Stop - Greater-Than Sign"});
    device.inputs.append({0, "Solidus - Question Mark"});
    device.inputs.append({0, "Kana N - Low Line"});
    device.inputs.append({0, "SHIFT Right"});
    device.inputs.append({0, "GRPH"});
    device.inputs.append({0, "Spacebar"});
    device.inputs.append({0, "CLR HOME"});
    device.inputs.append({0, "INS"});
    device.inputs.append({0, "DEL"});
    device.inputs.append({0, "Up"});
    device.inputs.append({0, "Left"});
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "Down"});
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::FamilyTrainer, "Family Trainer"};
    for(uint n : range(12)) {
      device.inputs.append({0, {"Button ", n + 1}});
    }
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::VausE, "Arkanoid Vaus"};
    device.inputs.append({1, "Control Knob"});
    device.inputs.append({0, "Fire Button" });
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::SFCGamepad, "SFC Gamepad"};
    device.inputs.append({0, "Up"    });
    device.inputs.append({0, "Down"  });
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    device.inputs.append({0, "B"     });
    device.inputs.append({0, "A"     });
    device.inputs.append({0, "Y"     });
    device.inputs.append({0, "X"     });
    device.inputs.append({0, "L"     });
    device.inputs.append({0, "R"     });
    device.inputs.append({0, "Select"});
    device.inputs.append({0, "Start" });
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::MouseE, "Mouse"};
    device.inputs.append({1, "X-axis"});
    device.inputs.append({1, "Y-axis"});
    device.inputs.append({0, "Left"  });
    device.inputs.append({0, "Right" });
    expansionPort.devices.append(device);
  }

  ports.append(move(hardware));
  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
  ports.append(move(expansionPort));
}

auto FamicomInterface::videoResolution() -> VideoSize {
  return {256, 240};
}

auto FamicomInterface::videoSize(uint width, uint height, bool arc, bool intScale) -> VideoSize {
  double w = 256;
  if(arc) {
    double squarePixelRate = Famicom::Region::NTSCJ() || Famicom::Region::NTSCU()
    ? 135.0 / 22.0 * 1'000'000.0
    : 7'375'000.0;
    w *= squarePixelRate / (system.frequency() / ppuM.rate());
  }
  int h = 240;
  double m;
  if(intScale) m = min((uint)(width / w), height / h);
  else         m = min(width / w, height / (double)h);
  return {(uint)(w * m), (uint)(h * m)};
}

auto FamicomInterface::videoColors() -> uint32 {
  return 1 << 9;
}

auto FamicomInterface::videoColor(uint32 n) -> uint64 {
  //YIQ decoder by Bisqwit (http://forums.nesdev.com/viewtopic.php?p=85060)
  static auto generateNTSCColor = [](uint9 n, double saturation, double hue, double contrast, double brightness, double gamma) -> uint64 {
    int color = n.bits(0,3), level = color < 0xe ? n.bits(4,5) : 1;

    static const double  lowLevels[4] = {0.350, 0.518, 0.962, 1.550};
    static const double highLevels[4] = {1.094, 1.506, 1.962, 1.962};

    static const double black = lowLevels[1], white = highLevels[3];
    static const double attenuation = 0.746;

    double lo_and_hi[2] = {
      (color == 0x0 ? highLevels : lowLevels)[level],
      (color <  0xd ? highLevels : lowLevels)[level],
    };

    double y = 0.0, i = 0.0, q = 0.0;
    auto wave = [](int phase, int color) -> bool { return (color + phase + 8) % 12 < 6; };
    for(int phase : range(12)) {
      double spot = lo_and_hi[wave(phase, color)];

      if(color < 0xe && (
         ((n.bit(6)) && wave(phase, 12))
      || ((n.bit(7)) && wave(phase,  4))
      || ((n.bit(8)) && wave(phase,  8))
      )) spot *= attenuation;

      double voltage = (spot - black) / (white - black);

      voltage = (voltage - 0.5) * contrast + 0.5;
      voltage *= brightness / 12.0;

      y += voltage;
      i += voltage * std::cos((Math::Pi / 6.0) * (phase + hue));
      q += voltage * std::sin((Math::Pi / 6.0) * (phase + hue));
    }

    i *= saturation;
    q *= saturation;

    auto gammaAdjust = [=](double f) -> double { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    //This matrix is from FCC's 1953 NTSC standard.
    //The Famicom and American NES are older than the SMPTE C standard that followed in 1987.
    uint64 r = uclamp<16>(65535.0 * gammaAdjust(y +  0.946882 * i +  0.623557 * q));
    uint64 g = uclamp<16>(65535.0 * gammaAdjust(y + -0.274788 * i + -0.635691 * q));
    uint64 b = uclamp<16>(65535.0 * gammaAdjust(y + -1.108545 * i +  1.709007 * q));

    return r << 32 | g << 16 | b << 0;
  };

  static auto generatePALColor = [](uint9 n, double saturation, double hue, double contrast, double brightness, double gamma) -> uint64 {
    int color = n.bits(0,3), level = color < 0xe ? n.bits(4,5) : 1;

    static const double  lowLevels[4] = {0.350, 0.518, 0.962, 1.550};
    static const double highLevels[4] = {1.094, 1.506, 1.962, 1.962};

    static const double black = lowLevels[1], white = highLevels[3];
    static const double attenuation = 0.746;

    double lo_and_hi[2] = {
      (color == 0x0 ? highLevels : lowLevels)[level],
      (color <  0xd ? highLevels : lowLevels)[level],
    };

    double ys[2] = {0.0, 0.0}, us[2] = {0.0, 0.0}, vs[2] = {0.0, 0.0};
    //Add 12 to work around C++ modulus implementation
    auto wave0 = [](int phase, int color) -> bool { return (color + 10 - phase + 12) % 12 < 6; };
    auto wave1 = [](int phase, int color) -> bool { return (color +  1 + phase +  0) % 12 < 6; };
    for(int phase : range(12)) {
      double spot[] = {lo_and_hi[wave0(phase, color)], lo_and_hi[wave1(phase, color)]};

      //swap red and green
      if(color < 0xe && (
         ((n.bit(6)) && wave0(phase,  4))
      || ((n.bit(7)) && wave0(phase, 12))
      || ((n.bit(8)) && wave0(phase,  8))
      )) spot[0] *= attenuation;

      if(color < 0xe && (
         ((n.bit(6)) && wave1(phase,  4))
      || ((n.bit(7)) && wave1(phase, 12))
      || ((n.bit(8)) && wave1(phase,  8))
      )) spot[1] *= attenuation;

      for(uint i : range(2)) {
        double voltage = (spot[i] - black) / (white - black);

        voltage = (voltage - 0.5) * contrast + 0.5;
        voltage *= brightness / 12.0;

        ys[i] += voltage;
        us[i] += voltage * std::cos((Math::Pi / 6.0) * (phase + hue));
        vs[i] += voltage * std::sin((Math::Pi / 6.0) * (phase + hue));
      }
    }

    double y = (ys[0] + ys[1]) / 2.0, u = (us[0] + us[1]) / 2.0, v = (vs[0] - vs[1]) / 2.0;

    u *= saturation;
    v *= saturation;

    auto gammaAdjust = [=](double f) -> double { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    uint64 r = uclamp<16>(65535.0 * gammaAdjust(y                 +  1.139837 * v));
    uint64 g = uclamp<16>(65535.0 * gammaAdjust(y + -0.394652 * u + -0.580599 * v));
    uint64 b = uclamp<16>(65535.0 * gammaAdjust(y +  2.032110 * u                ));

    return r << 32 | g << 16 | b << 0;
  };

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

  if(ppuM.ntsc()) {
    double saturation = 2.0;
    double hue = 0.0;
    double contrast = 1.0;
    double brightness = 1.0;
    double gamma = settings.colorEmulation ? 1.8 : 2.2;

    return generateNTSCColor(n & 0x1ff, saturation, hue, contrast, brightness, gamma);

  } else if(ppuM.pal()) {
    double saturation = 2.0;
    double hue = 0.0;
    double contrast = 1.0;
    double brightness = 1.0;
    double gamma = settings.colorEmulation ? 1.8 : 2.2;

    return generatePALColor(n & 0x1ff, saturation, hue, contrast, brightness, gamma);

  } else if(ppuM.rgb()) {
    const uint9* palette = nullptr;
    switch(ppuM.version) {
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
}

auto FamicomInterface::load(uint id) -> bool {
  return system.load(this, System::Model::Famicom);
}

auto FamicomInterface::connect(uint port, uint device) -> void {
  if(port == ID::Port::Controller1) {
    if(settings.controllerPort1 == ID::Device::FourScore) {
      platform->deviceChanged(ID::Port::Controller2, ID::Device::None);
    }
    controllerPortM1.connect(settings.controllerPort1 = device);
    if(device == ID::Device::FourScore) {
      platform->deviceChanged(ID::Port::Controller2, ID::Device::FourScore);
    }
  }

  if(port == ID::Port::Controller2) {
    if(settings.controllerPort2 == ID::Device::FourScore) {
      platform->deviceChanged(ID::Port::Controller1, ID::Device::None);
    }
    controllerPortM2.connect(settings.controllerPort2 = device);
    if(device == ID::Device::FourScore) {
      platform->deviceChanged(ID::Port::Controller1, ID::Device::FourScore);
    }
  }

  if(port == ID::Port::Expansion) expansionPort.connect(settings.expansionPort = device);
}
