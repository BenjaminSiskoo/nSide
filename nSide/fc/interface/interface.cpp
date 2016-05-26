#include <fc/fc.hpp>

namespace Famicom {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  system.init();

  information.manufacturer = "Nintendo";
  information.name         = "Famicom";
  information.canvasWidth  = 256; //increases to 512 during VS. DualSystem emulation
  information.canvasHeight = 240; //increases to 480 during PlayChoice-10 emulation
  information.overscan     = true;
  information.aspectRatio  = 8.0 / 7.0; //PAL: 2950000.0 / 2128137.0
  information.resettable   = true;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::Famicom,      "Famicom",       "fc",   true});
  media.append({ID::VSSystem,     "VS. System",    "vs",   true});
  media.append({ID::PlayChoice10, "PlayChoice-10", "pc10", true});
  //media.append({ID::FamicomBox,   "FamicomBox",    "fcb",  true});

  ports.append({0, "Port 1"});
  ports.append({1, "Port 2"});
  ports.append({2, "Expansion Port"});
  ports.append({3, "Arcade Panel"});

  { Device device{
      Famicom::Device::None,
      ID::ControllerPort1 | ID::ControllerPort2 | ID::ExpansionPort,
      "None"
    };
    addDevice(device);
  }

  { Device device{
      Famicom::Device::Gamepad,
      ID::ControllerPort1 | ID::ControllerPort2 | ID::ExpansionPort,
      "Gamepad"
    };
    device.inputs.append({0, 0, "Up"    });
    device.inputs.append({1, 0, "Down"  });
    device.inputs.append({2, 0, "Left"  });
    device.inputs.append({3, 0, "Right" });
    device.inputs.append({4, 0, "B"     });
    device.inputs.append({5, 0, "A"     });
    device.inputs.append({6, 0, "Select"});
    device.inputs.append({7, 0, "Start" });
    addDevice(device);
  }

  { Device device{
      Famicom::Device::FourPlayers,
      ID::ExpansionPort,
      "4-Players Adaptor"
    };
    for(uint p = 3, n = 0; p <= 4; p += 1, n += 8) {
      device.inputs.append({n + 0, 0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({n + 1, 0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({n + 2, 0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({n + 3, 0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({n + 4, 0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({n + 5, 0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({n + 6, 0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({n + 7, 0, {"Port ", p, " - ", "Start" }});
    }
    addDevice(device);
  }

  { Device device{
      Famicom::Device::FourScore,
      ID::ControllerPort1,
      "Four Score"
    };
    for(uint p = 1, n = 0; p <= 4; p += 2, n += 8) {
      device.inputs.append({n + 0, 0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({n + 1, 0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({n + 2, 0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({n + 3, 0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({n + 4, 0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({n + 5, 0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({n + 6, 0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({n + 7, 0, {"Port ", p, " - ", "Start" }});
    }
    addDevice(device);
  }

  { Device device{
      Famicom::Device::FourScore,
      ID::ControllerPort2,
      "Four Score"
    };
    for(uint p = 2, n = 0; p <= 4; p += 2, n += 8) {
      device.inputs.append({n + 0, 0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({n + 1, 0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({n + 2, 0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({n + 3, 0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({n + 4, 0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({n + 5, 0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({n + 6, 0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({n + 7, 0, {"Port ", p, " - ", "Start" }});
    }
    addDevice(device);
  }

  { Device device{
      Famicom::Device::BeamGun,
      ID::ExpansionPort,
      "Beam Gun"
    };
    device.inputs.append({0, 1, "X-axis" });
    device.inputs.append({1, 1, "Y-axis" });
    device.inputs.append({2, 0, "Trigger"});
    addDevice(device);
  }

  { Device device{
      Famicom::Device::BeamGun,
      ID::ControllerPort2,
      "Zapper"
    };
    device.inputs.append({0, 1, "X-axis" });
    device.inputs.append({1, 1, "Y-axis" });
    device.inputs.append({2, 0, "Trigger"});
    addDevice(device);
  }

  { Device device{
      Famicom::Device::FamilyTrainer,
      ID::ExpansionPort,
      "Family Trainer"
    };
    for(uint n = 0; n <= 11; n++) {
      device.inputs.append({n, 0, {"Button ", n + 1}});
    }
    addDevice(device);
  }

  { Device device{
      Famicom::Device::FamilyTrainer,
      ID::ControllerPort2,
      "Power Pad"
    };
    for(uint n = 0; n <= 11; n++) {
      device.inputs.append({n, 0, {"Button ", n + 1}});
    }
    addDevice(device);
  }

  { Device device{
      Famicom::Device::Vaus,
      ID::ControllerPort2 | ID::ExpansionPort,
      "Arkanoid Vaus"
    };
    device.inputs.append({0, 1, "Control Knob"});
    device.inputs.append({1, 0, "Fire Button" });
    addDevice(device);
  }

  { Device device{
      Famicom::Device::SFCGamepad,
      ID::ControllerPort1 | ID::ControllerPort2,
      "SFC Gamepad"
    };
    device.inputs.append({ 0, 0, "Up"    });
    device.inputs.append({ 1, 0, "Down"  });
    device.inputs.append({ 2, 0, "Left"  });
    device.inputs.append({ 3, 0, "Right" });
    device.inputs.append({ 4, 0, "B"     });
    device.inputs.append({ 5, 0, "A"     });
    device.inputs.append({ 6, 0, "Y"     });
    device.inputs.append({ 7, 0, "X"     });
    device.inputs.append({ 8, 0, "L"     });
    device.inputs.append({ 9, 0, "R"     });
    device.inputs.append({10, 0, "Select"});
    device.inputs.append({11, 0, "Start" });
    addDevice(device);
  }

  { Device device{
      Famicom::Device::Mouse,
      ID::ControllerPort1 | ID::ControllerPort2 | ID::ExpansionPort,
      "Mouse"
    };
    device.inputs.append({0, 1, "X-axis"});
    device.inputs.append({1, 1, "Y-axis"});
    device.inputs.append({2, 0, "Left"  });
    device.inputs.append({3, 0, "Right" });
    addDevice(device);
  }

  { Device device{
      Famicom::Device::VSPanel,
      ID::ArcadePanel,
      "VS. Panel"
    };
    device.inputs.append({0, 0, "Button 1"      });
    device.inputs.append({1, 0, "Button 2"      });
    device.inputs.append({2, 0, "Button 3"      });
    device.inputs.append({3, 0, "Button 4"      });
    device.inputs.append({4, 0, "Service Button"});
    device.inputs.append({5, 0, "Coin 1"        });
    device.inputs.append({6, 0, "Coin 2"        });
    addDevice(device);
  }
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC:  return system.cpuFrequency() / (262.0 * 1364.0 - 4.0);
  case System::Region::PAL:   return system.cpuFrequency() / (312.0 * 1705.0);
  case System::Region::Dendy: return system.cpuFrequency() / (312.0 * 1705.0);
  }
}

auto Interface::videoColors() -> uint32 {
  return ((1 << 9) << system.vs()) + system.pc10() * (1 << 8);
}

auto Interface::videoColor(uint32 n) -> uint64 {
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
    //Add 12 to work around C++ modulus implementation,
    //then 4 to compensate for reversed hue direction (cause unknown)
    auto wave0 = [](int phase, int color) -> bool { return (color + 6 - phase + 16) % 12 < 6; };
    auto wave1 = [](int phase, int color) -> bool { return (color + 9 - phase + 16) % 12 < 6; };
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

    double y = (ys[0] + ys[1]) / 2.0, u = (us[0] + us[1]) / 2.0, v = (vs[0] + vs[1]) / 2.0;

    u *= saturation;
    v *= saturation;

    auto gammaAdjust = [=](double f) -> double { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
    uint64 r = uclamp<16>(65535.0 * gammaAdjust(y                 +  1.139837 * v));
    uint64 g = uclamp<16>(65535.0 * gammaAdjust(y + -0.394652 * u + -0.580599 * v));
    uint64 b = uclamp<16>(65535.0 * gammaAdjust(y +  2.032110 * u                ));

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

  if(!system.pc10() || n < (1 << 9)) {
    if(ppu.ntsc()) { //NTSC
      double saturation = 2.0;
      double hue = 0.0;
      double contrast = 1.0;
      double brightness = 1.0;
      double gamma = settings.colorEmulation ? 1.8 : 2.2;

      return generateNTSCColor(n & 0x1ff, saturation, hue, contrast, brightness, gamma);

    } else if(ppu.pal()) { //PAL
      double saturation = 2.0;
      double hue = 0.0;
      double contrast = 1.0;
      double brightness = 1.0;
      double gamma = settings.colorEmulation ? 1.8 : 2.2;

      return generatePALColor(n & 0x1ff, saturation, hue, contrast, brightness, gamma);

    } else if(ppu.rgb()) { //Arcade
      const uint9* palette = nullptr;
      switch(ppu.revision) {
      case PPU::Revision::RP2C03B:
      case PPU::Revision::RP2C03G:
      case PPU::Revision::RC2C03B:
      case PPU::Revision::RC2C03C:
      case PPU::Revision::RC2C05_01:
      case PPU::Revision::RC2C05_02:
      case PPU::Revision::RC2C05_03:
      case PPU::Revision::RC2C05_04:
      case PPU::Revision::RC2C05_05:
        palette = PPU::RP2C03;
        break;
      case PPU::Revision::RP2C04_0001:
        palette = PPU::RP2C04_0001;
        break;
      case PPU::Revision::RP2C04_0002:
        palette = PPU::RP2C04_0002;
        break;
      case PPU::Revision::RP2C04_0003:
        palette = PPU::RP2C04_0003;
        break;
      case PPU::Revision::RP2C04_0004:
        palette = PPU::RP2C04_0004;
        break;
      }
      return generateRGBColor(n & 0x1ff, palette);
    }
  } else {
    return generatePC10Color(n - (1 << 9));
  }
}

auto Interface::audioFrequency() -> double {
  switch(system.region()) { default:
  case System::Region::NTSC:  return system.cpuFrequency() / 12.0;
  case System::Region::PAL:   return system.cpuFrequency() / 16.0;
  case System::Region::Dendy: return system.cpuFrequency() / 15.0;
  }
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::group(uint id) -> uint {
  switch(id) {
  case ID::SystemManifest:
  case ID::PC10BIOS:
  case ID::PC10CharacterROM:
  case ID::PC10PaletteROM:
  case ID::FamicomBoxPRG:
  case ID::FamicomBoxCHR:
    return ID::System;
  case ID::Manifest:
  case ID::ProgramROM:
  case ID::ProgramRAM:
  case ID::CharacterROM:
  case ID::CharacterRAM:
  case ID::ChipRAM:
    switch(system.revision()) {
    case System::Revision::Famicom:      return ID::Famicom;
    case System::Revision::VSSystem:     return ID::VSSystem;
    case System::Revision::PlayChoice10: return ID::PlayChoice10;
    case System::Revision::FamicomBox:   return ID::FamicomBox;
    }
  case ID::InstructionROM:
  case ID::KeyROM:
    return ID::PlayChoice10;
  }

  throw;
}

auto Interface::load(uint id) -> void {
  information.canvasWidth  = 256;
  information.canvasHeight = 240;
  information.aspectRatio = 8.0 / 7.0;
  if(id == ID::Famicom)      system.load(System::Revision::Famicom);
  if(id == ID::VSSystem)     system.load(System::Revision::VSSystem); 
  if(id == ID::PlayChoice10) system.load(System::Revision::PlayChoice10);
  if(id == ID::FamicomBox)   system.load(System::Revision::FamicomBox);
}

auto Interface::save() -> void {
  for(auto& memory : cartridge.memory) {
    saveRequest(memory.id, memory.name);
  }
}

auto Interface::load(uint id, const stream& stream) -> void {
  switch(id) {
  case ID::SystemManifest:
    system.information.manifest = stream.text();
    break;
  case ID::PC10BIOS:
    stream.read((uint8_t*)playchoice10.bios, min(16384u, stream.size()));
    break;
  case ID::PC10CharacterROM:
    stream.read((uint8_t*)playchoice10.videoCircuit.chrrom, min(24576u, stream.size()));
    break;
  case ID::PC10PaletteROM:
    stream.read((uint8_t*)playchoice10.videoCircuit.cgrom, min(768u, stream.size()));
    break;
  case ID::FamicomBoxPRG:
    stream.read((uint8_t*)famicombox.bios_prg, min(32768u, stream.size()));
    break;
  case ID::FamicomBoxCHR:
    stream.read((uint8_t*)famicombox.bios_chr, min(8192u, stream.size()));
    break;
  }

  if(id == ID::Manifest) cartridge.information.markup.cartridge = stream.text();
  if(id == ID::ProgramROM) cartridge.board->prgrom.read(stream);
  if(id == ID::ProgramRAM) cartridge.board->prgram.read(stream);
  if(id == ID::CharacterROM) cartridge.board->chrrom.read(stream);
  if(id == ID::CharacterRAM) cartridge.board->chrram.read(stream);
  if(id == ID::ChipRAM) cartridge.board->chip->ram.read(stream);

  if(id == ID::InstructionROM) cartridge.board->instrom.read(stream);
  if(id == ID::KeyROM) cartridge.board->keyrom.read(stream);
}

auto Interface::save(uint id, const stream& stream) -> void {
  if(id == ID::ProgramRAM) {
    stream.write((uint8_t*)cartridge.board->prgram.data(), cartridge.board->prgram.size());
  }

  if(id == ID::CharacterRAM) {
    stream.write((uint8_t*)cartridge.board->chrram.data(), cartridge.board->chrram.size());
  }

  if(id == ID::ChipRAM) {
    stream.write((uint8_t*)cartridge.board->chip->ram.data(), cartridge.board->chip->ram.size());
  }
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  if(system.vs() || port == Famicom::Port::Arcade) return;

  uint wideDevice = Famicom::Device::FourScore;
  if((port == Famicom::Port::Controller1 && settings.controllerPort1 == wideDevice)
  || (port == Famicom::Port::Controller2 && settings.controllerPort2 == wideDevice)) {
    switch(port) {
    case Famicom::Port::Controller1:
      interface->deviceChanged(Famicom::Port::Controller2, Famicom::Device::None);
      break;
    case Famicom::Port::Controller2:
      interface->deviceChanged(Famicom::Port::Controller1, Famicom::Device::None);
      break;
    }
  }

  Famicom::peripherals.connect(port, device);

  if(device == wideDevice) {
    if(port == Famicom::Port::Controller1) interface->deviceChanged(Famicom::Port::Controller2, wideDevice);
    if(port == Famicom::Port::Controller2) interface->deviceChanged(Famicom::Port::Controller1, wideDevice);
  }
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
  system.runToSave();
  return system.serialize();
}

auto Interface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto Interface::cheatSet(const lstring& list) -> void {
  cheat.reset();

  for(auto& codeset : list) {
    lstring codes = codeset.split("+");
    for(auto& code : codes) {
      lstring part = code.split("/");
      if(part.size() == 2) cheat.append(hex(part[0]), hex(part[1]));
      if(part.size() == 3) cheat.append(hex(part[0]), hex(part[1]), hex(part[2]));
    }
  }
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

auto Interface::addDevice(Device device) -> void {
  while(deviceRefs.size() <= device.id) {
    deviceRefs.append(DeviceRef{deviceRefs.size(), 0, 0, 0, 0});
  }
  for(auto& port : ports) {
    if(device.portmask & (1 << port.id)) {
      deviceRefs[device.id].port[port.id] = (uint)port.devices.size();
      port.devices.append(device);
    }
  }
}

auto Interface::exportMemory() -> void {
  string pathname = {path(group(ID::ProgramROM)), "debug/"};
  directory::create(pathname);

  // Registers
  string markup = "";
  ppu.exportRegisters(markup);
  file::write({pathname, "registers.bml"}, markup);

  file::write({pathname, "work.ram"}, (uint8_t*)cpu.ram, 0x0800);
  file::write({pathname, "video.ram"}, (uint8_t*)ppu.ciram, !system.vs() ? 2048 : 4096);
  file::write({pathname, "sprite.ram"}, (uint8_t*)ppu.oam, 256);
  file::write({pathname, "palette.ram"}, (uint8_t*)ppu.cgram, 32);
  if(cartridge.board->prgram.size()) saveRequest(ID::ProgramRAM, "debug/program.ram");
  if(cartridge.board->chrram.size()) saveRequest(ID::CharacterRAM, "debug/character.ram");
  if(cartridge.board->chip && cartridge.board->chip->ram.size()) {
    saveRequest(ID::ChipRAM, "debug/chip.ram");
  }
}

}
