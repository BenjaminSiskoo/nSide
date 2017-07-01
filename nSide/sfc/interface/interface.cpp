#include <sfc/sfc.hpp>

namespace SuperFamicom {

Settings settings;
Debugger debugger;

Interface::Interface() {
  system.init();

  information.devState     = DevState::Full;
  information.manufacturer = "Nintendo";
  information.name         = "Super Famicom";
  information.overscan     = true;

  media.append({ID::SuperFamicom, "Super Famicom", "sfc"});

  Port hardware{ID::Port::Hardware, "Hardware"};
  Port controllerPort1{ID::Port::Controller1, "Controller Port 1"};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2"};
  Port expansionPort{ID::Port::Expansion, "Expansion Port"};

  { Device device{ID::Device::Controls, "Controls"};
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

  { Device device{ID::Device::SuperMultitap, "Super Multitap"};
    for(uint p = 2; p <= 5; p++) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Down"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Left"  }});
      device.inputs.append({0, {"Port ", p, " - ", "Right" }});
      device.inputs.append({0, {"Port ", p, " - ", "B"     }});
      device.inputs.append({0, {"Port ", p, " - ", "A"     }});
      device.inputs.append({0, {"Port ", p, " - ", "Y"     }});
      device.inputs.append({0, {"Port ", p, " - ", "X"     }});
      device.inputs.append({0, {"Port ", p, " - ", "L"     }});
      device.inputs.append({0, {"Port ", p, " - ", "R"     }});
      device.inputs.append({0, {"Port ", p, " - ", "Select"}});
      device.inputs.append({0, {"Port ", p, " - ", "Start" }});
    }
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::SuperScope, "Super Scope"};
    device.inputs.append({1, "X-axis" });
    device.inputs.append({1, "Y-axis" });
    device.inputs.append({0, "Trigger"});
    device.inputs.append({0, "Cursor" });
    device.inputs.append({0, "Turbo"  });
    device.inputs.append({0, "Pause"  });
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Justifier, "Justifier"};
    device.inputs.append({1, "X-axis" });
    device.inputs.append({1, "Y-axis" });
    device.inputs.append({0, "Trigger"});
    device.inputs.append({0, "Start"  });
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Justifiers, "Justifiers"};
    device.inputs.append({1, "Port 1 - X-axis" });
    device.inputs.append({1, "Port 1 - Y-axis" });
    device.inputs.append({0, "Port 1 - Trigger"});
    device.inputs.append({0, "Port 1 - Start"  });
    device.inputs.append({1, "Port 2 - X-axis" });
    device.inputs.append({1, "Port 2 - Y-axis" });
    device.inputs.append({0, "Port 2 - Trigger"});
    device.inputs.append({0, "Port 2 - Start"  });
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Satellaview, "Satellaview"};
    expansionPort.devices.append(device);
  }

  { Device device{ID::Device::S21FX, "21fx"};
    expansionPort.devices.append(device);
  }

  ports.append(move(hardware));
  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
  ports.append(move(expansionPort));
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoResolution() -> VideoSize {
  return {512, 480};
}

auto Interface::videoSize(uint width, uint height, bool arc, bool intScale) -> VideoSize {
  double w = 256;
  double h = 240;
  double m;

  if(arc) {
    double squarePixelRate = system.region() == System::Region::NTSC
    ? 135.0 / 22.0 * 1'000'000.0
    : 7'375'000.0;
    w *= squarePixelRate / (system.cpuFrequency() / (2.0 + 2.0));
  }

  if(intScale) m = min(  (uint)(width / w),   (uint)(height / h));
  else         m = min((double)(width / w), (double)(height / h));

  return {(uint)(w * m), (uint)(h * m)};
}

auto Interface::videoColors() -> uint32 {
  return 1 << 19;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint r = color.bits( 0, 4);
  uint g = color.bits( 5, 9);
  uint b = color.bits(10,14);
  uint l = color.bits(15,18);

  //luma=0 is not 100% black; but it's much darker than normal linear scaling
  //exact effect seems to be analog; requires > 24-bit color depth to represent accurately
  double L = (1.0 + l) / 16.0 * (l ? 1.0 : 0.25);
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

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::load(uint id) -> bool {
  if(id == ID::SuperFamicom) return system.load(this);
  if(id == ID::BSMemory) return cartridge.loadBSMemory();
  if(id == ID::SufamiTurboA) return cartridge.loadSufamiTurboA();
  if(id == ID::SufamiTurboB) return cartridge.loadSufamiTurboB();
  return false;
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  if(port == ID::Port::Controller1) controllerPort1.connect(settings.controllerPort1 = device);
  if(port == ID::Port::Controller2) controllerPort2.connect(settings.controllerPort2 = device);
  if(port == ID::Port::Expansion) expansionPort.connect(settings.expansionPort = device);
}

auto Interface::power() -> void {
  system.power();
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::rtc() -> bool {
  if(cartridge.has.EpsonRTC) return true;
  if(cartridge.has.SharpRTC) return true;
  return false;
}

auto Interface::rtcSynchronize() -> void {
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

auto Interface::cheatSet(const string_vector& list) -> void {
  cheat.reset();
  #if defined(SFC_SUPERGAMEBOY)
  if(cartridge.has.ICD2) return GameBoy::cheat.assign(list);
  #endif
  cheat.assign(list);
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
  string pathname = {platform->path(cartridge.pathID()), "debug/"};
  directory::create(pathname);

  if(auto fp = platform->open(cartridge.pathID(), "debug/work.ram", File::Write)) fp->write(cpu.wram, 128 * 1024);
  if(cartridge.ram.size()) if(auto fp = platform->open(cartridge.pathID(), "debug/program-save.ram", File::Write)) {
    fp->write(cartridge.ram.data(), cartridge.ram.size());
  }
  if(cartridge.has.MCC) if(auto fp = platform->open(cartridge.pathID(), "debug/mcc.ram", File::Write)) {
    fp->write(mcc.ram.data(), mcc.ram.size());
  }
  if(cartridge.has.Event) if(auto fp = platform->open(cartridge.pathID(), "debug/event.ram", File::Write)) {
    fp->write(event.ram.data(), event.ram.size());
  }
  if(cartridge.has.SA1) {
    if(auto fp = platform->open(cartridge.pathID(), "debug/sa1.internal.ram", File::Write)) {
      fp->write(sa1.iram.data(), sa1.iram.size());
    }
    if(auto fp = platform->open(cartridge.pathID(), "debug/sa1.bitmap-work.ram", File::Write)) {
      fp->write(sa1.bwram.data(), sa1.bwram.size());
    }
  }
  if(cartridge.has.SuperFX) if(auto fp = platform->open(cartridge.pathID(), "debug/superfx.ram", File::Write)) {
    fp->write(superfx.ram.data(), superfx.ram.size());
  }
  if(cartridge.has.SPC7110) if(auto fp = platform->open(cartridge.pathID(), "debug/spc7110.ram", File::Write)) {
    fp->write(spc7110.ram.data(), spc7110.ram.size());
  }
  if(cartridge.has.SDD1) if(auto fp = platform->open(cartridge.pathID(), "debug/sdd1.ram", File::Write)) {
    fp->write(sdd1.ram.data(), sdd1.ram.size());
  }
  if(cartridge.has.OBC1) if(auto fp = platform->open(cartridge.pathID(), "debug/obc1.ram", File::Write)) {
    fp->write(obc1.ram.data(), obc1.ram.size());
  }

  if(cartridge.has.SufamiTurboSlots) {
    if(auto fp = platform->open(cartridge.pathID(), "debug/sufamiturbo.slota.ram", File::Write)) {
      fp->write(sufamiturboA.ram.data(), sufamiturboA.ram.size());
    }
    if(auto fp = platform->open(cartridge.pathID(), "debug/sufamiturbo.slotb.ram", File::Write)) {
      fp->write(sufamiturboB.ram.data(), sufamiturboB.ram.size());
    }
  }
}

}
