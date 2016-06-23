#include <sfc/sfc.hpp>

namespace SuperFamicom {

Interface* interface = nullptr;
Settings settings;

Interface::Interface() {
  interface = this;
  system.init();

  information.manufacturer = "Nintendo";
  information.name         = "Super Famicom";
  information.canvasWidth  = 256;  //note: half of the true size of 512×480,
  information.canvasHeight = 240;  //needed for Medium (×3) scale
  information.overscan     = true;
  information.aspectRatio  = 8.0 / 7.0;
  information.resettable   = true;

  information.capability.states = true;
  information.capability.cheats = true;

  media.append({ID::SuperFamicom, "Super Famicom", "sfc", true });
  media.append({ID::SuperFamicom, "Game Boy",      "gb",  false});
  media.append({ID::SuperFamicom, "BS Memory",     "bs",  false});
  media.append({ID::SuperFamicom, "Sufami Turbo",  "st",  false});

  ports.append({0, "Controller Port 1", true});
  ports.append({1, "Controller Port 2", true});
  ports.append({2, "Expansion Port",    true});

  auto& controllerPort1 = ports[0].devices;
  auto& controllerPort2 = ports[1].devices;
  auto& expansionPort   = ports[2].devices;

  { Device device{"None"};
    controllerPort1.append(device);
    controllerPort2.append(device);
    expansionPort.append(device);
  }

  { Device device{"Gamepad"};
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
    controllerPort1.append(device);
    controllerPort2.append(device);
  }

  { Device device{"Multitap"};
    for(uint p = 1, n = 0; p <= 4; p++, n += 12) {
      device.inputs.append({n +  0, 0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({n +  1, 0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({n +  2, 0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({n +  3, 0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({n +  4, 0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({n +  5, 0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({n +  6, 0, {"Port ", p, " - ", "Y"     }});
      device.inputs.append({n +  7, 0, {"Port ", p, " - ", "X"     }});
      device.inputs.append({n +  8, 0, {"Port ", p, " - ", "L"     }});
      device.inputs.append({n +  9, 0, {"Port ", p, " - ", "R"     }});
      device.inputs.append({n + 10, 0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({n + 11, 0, {"Port ", p, " - ", "Start" }});
    }
    controllerPort1.append(device);
    controllerPort2.append(device);
  }

  { Device device{"Mouse"};
    device.inputs.append({0, 1, "X-axis"});
    device.inputs.append({1, 1, "Y-axis"});
    device.inputs.append({2, 0, "Left"  });
    device.inputs.append({3, 0, "Right" });
    controllerPort1.append(device);
    controllerPort2.append(device);
  }

  { Device device{"Super Scope"};
    device.inputs.append({0, 1, "X-axis" });
    device.inputs.append({1, 1, "Y-axis" });
    device.inputs.append({2, 0, "Trigger"});
    device.inputs.append({3, 0, "Cursor" });
    device.inputs.append({4, 0, "Turbo"  });
    device.inputs.append({5, 0, "Pause"  });
    controllerPort2.append(device);
  }

  { Device device{"Justifier"};
    device.inputs.append({0, 1, "X-axis" });
    device.inputs.append({1, 1, "Y-axis" });
    device.inputs.append({2, 0, "Trigger"});
    device.inputs.append({3, 0, "Start"  });
    controllerPort2.append(device);
  }

  { Device device{"Justifiers"};
    device.inputs.append({0, 1, "Port 1 - X-axis" });
    device.inputs.append({1, 1, "Port 1 - Y-axis" });
    device.inputs.append({2, 0, "Port 1 - Trigger"});
    device.inputs.append({3, 0, "Port 1 - Start"  });
    device.inputs.append({4, 1, "Port 2 - X-axis" });
    device.inputs.append({5, 1, "Port 2 - Y-axis" });
    device.inputs.append({6, 0, "Port 2 - Trigger"});
    device.inputs.append({7, 0, "Port 2 - Start"  });
    controllerPort2.append(device);
  }

  { Device device{"Satellaview"};
    expansionPort.append(device);
  }

  { Device device{"Super Disc"};
    expansionPort.append(device);
  }

  { Device device{"21fx"};
    expansionPort.append(device);
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
  case System::Region::NTSC: return system.cpuFrequency() / (262.0 * 1364.0 - 4.0);
  case System::Region::PAL:  return system.cpuFrequency() / (312.0 * 1364.0);
  }
}

auto Interface::videoColors() -> uint32 {
  return 1 << 19;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint r = color.bits( 0, 4);
  uint g = color.bits( 5, 9);
  uint b = color.bits(10,14);
  uint l = color.bits(15,18);

  double L = (1.0 + l) / 16.0 * (l ? 1.0 : 0.5);
  uint64 R = L * image::normalize(r, 5, 16);
  uint64 G = L * image::normalize(g, 5, 16);
  uint64 B = L * image::normalize(b, 5, 16);

  if(settings.colorEmulation) {
    static const uint8 gammaRamp[32] = {
      0x00, 0x01, 0x03, 0x06, 0x0a, 0x0f, 0x15, 0x1c,
      0x24, 0x2d, 0x37, 0x42, 0x4e, 0x5b, 0x69, 0x78,
      0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8, 0xc0,
      0xc8, 0xd0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8, 0xff,
    };
    R = L * gammaRamp[r] * 0x0101;
    G = L * gammaRamp[g] * 0x0101;
    B = L * gammaRamp[b] * 0x0101;
  }

  return R << 32 | G << 16 | B << 0;
}

auto Interface::audioFrequency() -> double {
  return system.apuFrequency() / 768.0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

//deprecated
auto Interface::group(uint id) -> uint {
  switch(id) {
  case ID::SuperGameBoyManifest:
  case ID::SuperGameBoyBootROM:
    return 1;
  case ID::GameBoyManifest:
  case ID::GameBoyROM:
  case ID::GameBoyRAM:
    return 2;
  }
  throw;
}

auto Interface::load(uint id) -> void {
  if(id == ID::SuperFamicom) system.load();
  if(id == ID::BSMemory) cartridge.loadBSMemory();
  if(id == ID::SufamiTurboA) cartridge.loadSufamiTurboA();
  if(id == ID::SufamiTurboB) cartridge.loadSufamiTurboB();
}

auto Interface::save() -> void {
  system.save();
}

//deprecated
auto Interface::load(uint id, const stream& stream) -> void {
  #if defined(SFC_SUPERGAMEBOY)
  if(id == ID::SuperGameBoyManifest) {
    GameBoy::interface->load(GameBoy::ID::SystemManifest, stream);
  }

  if(id == ID::SuperGameBoyBootROM) {
    GameBoy::interface->load(GameBoy::ID::SuperGameBoyBootROM, stream);
  }

  if(id == ID::GameBoyManifest) {
    GameBoy::interface->load(GameBoy::ID::Manifest, stream);
  }

  if(id == ID::GameBoyROM) {
    GameBoy::interface->load(GameBoy::ID::ROM, stream);
  }

  if(id == ID::GameBoyRAM) {
    GameBoy::interface->load(GameBoy::ID::RAM, stream);
  }
  #endif
}

//deprecated
auto Interface::save(uint id, const stream& stream) -> void {
  #if defined(SFC_SUPERGAMEBOY)
  if(id == ID::GameBoyRAM) {
    GameBoy::interface->save(GameBoy::ID::RAM, stream);
  }
  #endif
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  SuperFamicom::peripherals.connect(port, device);
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

auto Interface::rtc() -> bool {
  if(cartridge.has.EpsonRTC) return true;
  if(cartridge.has.SharpRTC) return true;
  return false;
}

auto Interface::rtcsync() -> void {
  if(cartridge.has.EpsonRTC) epsonrtc.sync();
  if(cartridge.has.SharpRTC) sharprtc.sync();
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

  #if defined(SFC_SUPERGAMEBOY)
  if(cartridge.has.ICD2) {
    GameBoy::cheat.reset();
    for(auto& codeset : list) {
      lstring codes = codeset.split("+");
      for(auto& code : codes) {
        lstring part = code.split("/");
        if(part.size() == 2) GameBoy::cheat.append(hex(part[0]), hex(part[1]));
        if(part.size() == 3) GameBoy::cheat.append(hex(part[0]), hex(part[1]), hex(part[2]));
      }
    }
    return;
  }
  #endif

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
  if(name == "Blur Emulation") return true;
  if(name == "Color Emulation") return true;
  if(name == "Scanline Emulation") return true;
  return false;
}

auto Interface::get(const string& name) -> any {
  if(name == "Blur Emulation") return settings.blurEmulation;
  if(name == "Color Emulation") return settings.colorEmulation;
  if(name == "Scanline Emulation") return settings.scanlineEmulation;
  return {};
}

auto Interface::set(const string& name, const any& value) -> bool {
  if(name == "Blur Emulation" && value.is<bool>()) {
    settings.blurEmulation = value.get<bool>();
    system.configureVideoEffects();
    return true;
  }
  if(name == "Color Emulation" && value.is<bool>()) {
    settings.colorEmulation = value.get<bool>();
    system.configureVideoPalette();
    return true;
  }
  if(name == "Scanline Emulation" && value.is<bool>()) return settings.scanlineEmulation = value.get<bool>(), true;
  return false;
}

auto Interface::exportMemory() -> void {
  string pathname = {path(1), "debug/"};
  directory::create(pathname);

  if(auto fp = interface->open(1, "debug/work.ram", File::Write)) fp->write(cpu.wram, 128 * 1024);
  if(cartridge.ram.size()) if(auto fp = interface->open(1, "debug/program-save.ram", File::Write)) {
    fp->write(cartridge.ram.data(), cartridge.ram.size());
  }
  if(cartridge.has.MCC) if(auto fp = interface->open(1, "debug/mcc.ram", File::Write)) {
    fp->write(mcc.ram.data(), mcc.ram.size());
  }
  if(cartridge.has.Event) if(auto fp = interface->open(1, "debug/event.ram", File::Write)) {
    fp->write(event.ram.data(), event.ram.size());
  }
  if(cartridge.has.SA1) {
    if(auto fp = interface->open(1, "debug/sa1.internal.ram", File::Write)) {
      fp->write(sa1.iram.data(), sa1.iram.size());
    }
    if(auto fp = interface->open(1, "debug/sa1.bitmap-work.ram", File::Write)) {
      fp->write(sa1.bwram.data(), sa1.bwram.size());
    }
  }
  if(cartridge.has.SuperFX) if(auto fp = interface->open(1, "debug/superfx.ram", File::Write)) {
    fp->write(superfx.ram.data(), superfx.ram.size());
  }
  if(cartridge.has.SPC7110) if(auto fp = interface->open(1, "debug/spc7110.ram", File::Write)) {
    fp->write(spc7110.ram.data(), spc7110.ram.size());
  }
  if(cartridge.has.SDD1) if(auto fp = interface->open(1, "debug/sdd1.ram", File::Write)) {
    fp->write(sdd1.ram.data(), sdd1.ram.size());
  }
  if(cartridge.has.OBC1) if(auto fp = interface->open(1, "debug/obc1.ram", File::Write)) {
    fp->write(obc1.ram.data(), obc1.ram.size());
  }

  if(cartridge.has.SufamiTurboSlots) {
    if(auto fp = interface->open(1, "debug/sufamiturbo.slota.ram", File::Write)) {
      fp->write(sufamiturboA.ram.data(), sufamiturboA.ram.size());
    }
    if(auto fp = interface->open(1, "debug/sufamiturbo.slotb.ram", File::Write)) {
      fp->write(sufamiturboB.ram.data(), sufamiturboB.ram.size());
    }
  }
}

}
