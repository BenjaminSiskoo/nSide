#include <md/md.hpp>

namespace MegaDrive {

Settings settings;

Interface::Interface() {
  information.devState     = DevState::Alpha;
  information.manufacturer = "Sega";
  information.name         = "Mega Drive";
  information.overscan     = true;

  regions.append({"Autodetect"});
  regions.append({"NTSC-J"});
  regions.append({"NTSC-U"});
  regions.append({"PAL"});

  media.append({ID::MegaDrive, "Mega Drive", "md"});

  Port controllerPort1{ID::Port::Controller1, "Controller Port 1"};
  Port controllerPort2{ID::Port::Controller2, "Controller Port 2"};
  Port extensionPort{ID::Port::Extension, "Extension Port"};
  Port hardware{ID::Port::Hardware, "Hardware"};

  { Device device{ID::Device::None, "None"};
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
    extensionPort.devices.append(device);
  }

  { Device device{ID::Device::ControlPad, "Control Pad"};
    device.inputs.append({0, "Up"   });
    device.inputs.append({0, "Down" });
    device.inputs.append({0, "Left" });
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "A"    });
    device.inputs.append({0, "B"    });
    device.inputs.append({0, "C"    });
    device.inputs.append({0, "Start"});
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::FightingPad, "Fighting Pad"};
    device.inputs.append({0, "Up"   });
    device.inputs.append({0, "Down" });
    device.inputs.append({0, "Left" });
    device.inputs.append({0, "Right"});
    device.inputs.append({0, "A"    });
    device.inputs.append({0, "B"    });
    device.inputs.append({0, "C"    });
    device.inputs.append({0, "X"    });
    device.inputs.append({0, "Y"    });
    device.inputs.append({0, "Z"    });
    device.inputs.append({0, "Mode" });
    device.inputs.append({0, "Start"});
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::SegaTap, "Sega Tap"};
    for(char p = 'A'; p <= 'D'; p++) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"   }});
      device.inputs.append({0, {"Port ", p, " - ", "Down" }});
      device.inputs.append({0, {"Port ", p, " - ", "Left" }});
      device.inputs.append({0, {"Port ", p, " - ", "Right"}});
      device.inputs.append({0, {"Port ", p, " - ", "A"    }});
      device.inputs.append({0, {"Port ", p, " - ", "B"    }});
      device.inputs.append({0, {"Port ", p, " - ", "C"    }});
      device.inputs.append({0, {"Port ", p, " - ", "X"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Y"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Z"    }});
      device.inputs.append({0, {"Port ", p, " - ", "Mode" }});
      device.inputs.append({0, {"Port ", p, " - ", "Start"}});
    }
    controllerPort1.devices.append(device);
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::EA4WayPlay, "EA 4 Way Play"};
    for(uint p = 1; p <= 4; p++) {
      device.inputs.append({0, {"Port ", p, " - ", "Up"   }});
      device.inputs.append({0, {"Port ", p, " - ", "Down" }});
      device.inputs.append({0, {"Port ", p, " - ", "Left" }});
      device.inputs.append({0, {"Port ", p, " - ", "Right"}});
      device.inputs.append({0, {"Port ", p, " - ", "A"    }});
      device.inputs.append({0, {"Port ", p, " - ", "B"    }});
      device.inputs.append({0, {"Port ", p, " - ", "C"    }});
    //device.inputs.append({0, {"Port ", p, " - ", "X"    }});
    //device.inputs.append({0, {"Port ", p, " - ", "Y"    }});
    //device.inputs.append({0, {"Port ", p, " - ", "Z"    }});
    //device.inputs.append({0, {"Port ", p, " - ", "Mode" }});
      device.inputs.append({0, {"Port ", p, " - ", "Start"}});
    }
    controllerPort1.devices.append(device);
  }

  { Device device{ID::Device::EA4WayPlay, "EA 4 Way Play"};
    controllerPort2.devices.append(device);
  }

  { Device device{ID::Device::Controls, "Controls"};
    device.inputs.append({0, "Reset"});
    hardware.devices.append(device);
  }

  ports.append(move(controllerPort1));
  ports.append(move(controllerPort2));
  ports.append(move(extensionPort));
  ports.append(move(hardware));
}

auto Interface::manifest() -> string {
  return cartridge.manifest();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::videoResolution() -> VideoResolution {
  double squarePixelRate = !MegaDrive::Region::PAL()
  ? 135.0 / 22.0 * 1'000'000.0
  : 7'375'000.0;
  double pixelAspectRatio = squarePixelRate / (system.frequency() / 8.0);
  return {320, 240, 1280, 480, pixelAspectRatio};
}

auto Interface::videoColors() -> uint32 {
  return (1 << 9) * 3;
}

auto Interface::videoColor(uint32 color) -> uint64 {
  uint R = color.bits(0,2);
  uint G = color.bits(3,5);
  uint B = color.bits(6,8);
  uint intensity = color.bits(9,10);

  switch(intensity) {
  case 0:  //shadow
    break;
  case 1:  //normal
    R <<= 1;
    G <<= 1;
    B <<= 1;
    break;
  case 2:  //highlight
    R += 7;
    G += 7;
    B += 7;
    break;
  }

  uint64 r = image::normalize(R, 4, 16) + image::normalize(R, 4, 16) / 14;
  uint64 g = image::normalize(G, 4, 16) + image::normalize(G, 4, 16) / 14;
  uint64 b = image::normalize(B, 4, 16) + image::normalize(B, 4, 16) / 14;

  return r << 32 | g << 16 | b << 0;
}

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::sha256() -> string {
  return cartridge.sha256();
}

auto Interface::load(uint id) -> bool {
  return system.load(this);
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::connect(uint port, uint device) -> void {
  if(port == ID::Port::Controller1) {
    if(settings.controllerPort1 == ID::Device::EA4WayPlay) {
      platform->deviceChanged(ID::Port::Controller2, ID::Device::None);
    }
    controllerPort1.connect(settings.controllerPort1 = device);
    if(device == ID::Device::EA4WayPlay) {
      platform->deviceChanged(ID::Port::Controller2, ID::Device::EA4WayPlay);
    }
  }

  if(port == ID::Port::Controller2) {
    if(settings.controllerPort2 == ID::Device::EA4WayPlay) {
      platform->deviceChanged(ID::Port::Controller1, ID::Device::None);
    }
    controllerPort2.connect(settings.controllerPort2 = device);
    if(device == ID::Device::EA4WayPlay) {
      platform->deviceChanged(ID::Port::Controller1, ID::Device::EA4WayPlay);
    }
  }

  if(port == ID::Port::Extension) extensionPort.connect(settings.extensionPort = device);
}

auto Interface::power() -> void {
  system.power();
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

auto Interface::cheatSet(const string_vector& list) -> void {
  cheat.assign(list);
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
