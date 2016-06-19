Peripherals peripherals;

auto Peripherals::unload() -> void {
  delete controllerPort1;
  delete controllerPort2;
  delete expansionPort;
  controllerPort1 = nullptr;
  controllerPort2 = nullptr;
  expansionPort = nullptr;
}

auto Peripherals::reset() -> void {
  connect(0, settings.controllerPort1);
  connect(1, settings.controllerPort2);
  connect(2, settings.expansionPort);
  connect(3, settings.arcadePanel);
}

auto Peripherals::connect(uint port, uint id) -> void {
  auto index = [&]() -> uint {
    for(uint index : range(interface->ports[port].devices.size())) {
      if(interface->ports[port].devices[index].id == id) return index;
    }
    return 0;
  };

  if(port == Port::Controller1) {
    settings.controllerPort1 = id;
    if(!system.loaded()) return;

    delete controllerPort1;
    switch(id) { default:
    case Device::None:        controllerPort1 = new Controller(0, index()); break;
    case Device::Gamepad:     controllerPort1 = new Gamepad(0, index()); break;
    case Device::FourScore13: controllerPort1 = new FourScore(0, index()); break;
    case Device::SNESGamepad: controllerPort1 = new SNESGamepad(0, index()); break;
    case Device::Mouse:       controllerPort1 = new Mouse(0, index()); break;
    }

    if(id == Device::FourScore13 && settings.controllerPort2 != Device::FourScore24) {
      connect(Port::Controller2, Device::FourScore24);
    } else if(id != Device::FourScore13 && settings.controllerPort2 == Device::FourScore24) {
      connect(Port::Controller2, Device::None);
    }
  }

  if(port == Port::Controller2) {
    settings.controllerPort2 = id;
    if(!system.loaded()) return;

    delete controllerPort2;
    switch(id) { default:
    case Device::None:        controllerPort2 = new Controller(1, index()); break;
    case Device::Gamepad:     controllerPort2 = new Gamepad(1, index()); break;
    case Device::FourScore24: controllerPort2 = new FourScore(1, index()); break;
    case Device::Zapper:      controllerPort2 = new Zapper(1, index()); break;
    case Device::PowerPad:    controllerPort2 = new PowerPad(1, index()); break;
    case Device::Vaus:        controllerPort2 = new Vaus(1, index()); break;
    case Device::SNESGamepad: controllerPort2 = new SNESGamepad(1, index()); break;
    case Device::Mouse:       controllerPort2 = new Mouse(1, index()); break;
    }

    if(id == Device::FourScore24 && settings.controllerPort1 != Device::FourScore13) {
      connect(Port::Controller1, Device::FourScore13);
    } else if(id != Device::FourScore24 && settings.controllerPort1 == Device::FourScore13) {
      connect(Port::Controller1, Device::None);
    }
  }

  if(port == Port::Expansion) {
    settings.expansionPort = id;
    if(!system.loaded()) return;

    delete expansionPort;
    switch(id) { default:
    case Device::None:          expansionPort = new Expansion(index()); break;
    case Device::GamepadE:      expansionPort = new GamepadE(index()); break;
    case Device::FourPlayers:   expansionPort = new FourPlayers(index()); break;
    case Device::BeamGun:       expansionPort = new BeamGun(index()); break;
    case Device::FamilyTrainer: expansionPort = new FamilyTrainer(index()); break;
    case Device::VausE:         expansionPort = new VausE(index()); break;
    case Device::SFCGamepad:    expansionPort = new SFCGamepad(index()); break;
    case Device::MouseE:        expansionPort = new MouseE(index()); break;
    }
  }

  if(port == Port::Arcade) {
    settings.arcadePanel = id;
    if(!system.loaded()) return;
  }

  cpu.peripherals.reset();
  cpu.peripherals.append(controllerPort1);
  cpu.peripherals.append(controllerPort2);
  cpu.peripherals.append(expansionPort);
}
