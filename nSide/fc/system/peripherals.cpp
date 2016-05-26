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
  if(port == Port::Controller1) {
    settings.controllerPort1 = id;
    if(!system.loaded()) return;

    delete controllerPort1;
    switch(id) { default:
    case Device::None:       controllerPort1 = new Controller(0); break;
    case Device::Gamepad:    controllerPort1 = new Gamepad(0); break;
    case Device::FourScore:  controllerPort1 = new FourScore(0); break;
    case Device::SFCGamepad: controllerPort1 = new SFCGamepad(0); break;
    case Device::Mouse:      controllerPort1 = new Mouse(0); break;
    }

    if(id == Device::FourScore && settings.controllerPort2 != Device::FourScore) {
      connect(Port::Controller2, Device::FourScore);
    } else if(id != Device::FourScore && settings.controllerPort2 == Device::FourScore) {
      connect(Port::Controller2, Device::None);
    }
  }

  if(port == Port::Controller2) {
    settings.controllerPort2 = id;
    if(!system.loaded()) return;

    delete controllerPort2;
    switch(id) { default:
    case Device::None:          controllerPort2 = new Controller(1); break;
    case Device::Gamepad:       controllerPort2 = new Gamepad(1); break;
    case Device::FourScore:     controllerPort2 = new FourScore(1); break;
    case Device::BeamGun:       controllerPort2 = new BeamGun(1); break;
    case Device::FamilyTrainer: controllerPort2 = new FamilyTrainer(1); break;
    case Device::Vaus:          controllerPort2 = new Vaus(1); break;
    case Device::SFCGamepad:    controllerPort2 = new SFCGamepad(1); break;
    case Device::Mouse:         controllerPort2 = new Mouse(1); break;
    }

    if(id == Device::FourScore && settings.controllerPort1 != Device::FourScore) {
      connect(Port::Controller1, Device::FourScore);
    } else if(id != Device::FourScore && settings.controllerPort1 == Device::FourScore) {
      connect(Port::Controller1, Device::None);
    }
  }

  if(port == Port::Expansion) {
    settings.expansionPort = id;
    if(!system.loaded()) return;

    delete expansionPort;
    switch(id) { default:
    case Device::None:          expansionPort = new Controller(2); break;
    case Device::Gamepad:       expansionPort = new Gamepad(2); break;
    case Device::FourPlayers:   expansionPort = new FourPlayers(2); break;
    case Device::BeamGun:       expansionPort = new BeamGun(2); break;
    case Device::FamilyTrainer: expansionPort = new FamilyTrainer(2); break;
    case Device::Vaus:          expansionPort = new Vaus(2); break;
    case Device::Mouse:         expansionPort = new Mouse(2); break;
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
  system.configureVideoCursors();
}
