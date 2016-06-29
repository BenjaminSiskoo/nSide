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
  connect(ID::Port::Controller1, settings.controllerPort1);
  connect(ID::Port::Controller2, settings.controllerPort2);
  connect(ID::Port::Expansion, settings.expansionPort);
  connect(ID::Port::Arcade, settings.arcadePanel);
}

auto Peripherals::connect(uint port, uint device) -> void {
  if(port == ID::Port::Controller1) {
    settings.controllerPort1 = device;
    if(!system.loaded()) return;

    delete controllerPort1;
    switch(device) { default:
    case ID::Device::None:        controllerPort1 = new Controller(0); break;
    case ID::Device::Gamepad:     controllerPort1 = new Gamepad(0); break;
    case ID::Device::FourScore:   controllerPort1 = new FourScore(0); break;
    case ID::Device::SNESGamepad: controllerPort1 = new SNESGamepad(0); break;
    case ID::Device::Mouse:       controllerPort1 = new Mouse(0); break;
    }

    if(device == ID::Device::FourScore && settings.controllerPort2 != ID::Device::FourScore) {
      connect(ID::Port::Controller2, ID::Device::FourScore);
    } else if(device != ID::Device::FourScore && settings.controllerPort2 == ID::Device::FourScore) {
      connect(ID::Port::Controller2, ID::Device::None);
    }
  }

  if(port == ID::Port::Controller2) {
    settings.controllerPort2 = device;
    if(!system.loaded()) return;

    delete controllerPort2;
    switch(device) { default:
    case ID::Device::None:        controllerPort2 = new Controller(1); break;
    case ID::Device::Gamepad:     controllerPort2 = new Gamepad(1); break;
    case ID::Device::GamepadMic:  controllerPort2 = new GamepadMic(1); break;
    case ID::Device::FourScore:   controllerPort2 = new FourScore(1); break;
    case ID::Device::Zapper:      controllerPort2 = new Zapper(1); break;
    case ID::Device::PowerPad:    controllerPort2 = new PowerPad(1); break;
    case ID::Device::Vaus:        controllerPort2 = new Vaus(1); break;
    case ID::Device::SNESGamepad: controllerPort2 = new SNESGamepad(1); break;
    case ID::Device::Mouse:       controllerPort2 = new Mouse(1); break;
    }

    if(device == ID::Device::FourScore && settings.controllerPort1 != ID::Device::FourScore) {
      connect(ID::Port::Controller1, ID::Device::FourScore);
    } else if(device != ID::Device::FourScore && settings.controllerPort1 == ID::Device::FourScore) {
      connect(ID::Port::Controller1, ID::Device::None);
    }
  }

  if(port == ID::Port::Expansion) {
    settings.expansionPort = device;
    if(!system.loaded()) return;

    delete expansionPort;
    switch(device) { default:
    case ID::Device::None:          expansionPort = new Expansion; break;
    case ID::Device::GamepadE:      expansionPort = new GamepadE; break;
    case ID::Device::FourPlayers:   expansionPort = new FourPlayers; break;
    case ID::Device::BeamGun:       expansionPort = new BeamGun; break;
    case ID::Device::FamilyTrainer: expansionPort = new FamilyTrainer; break;
    case ID::Device::VausE:         expansionPort = new VausE; break;
    case ID::Device::SFCGamepad:    expansionPort = new SFCGamepad; break;
    case ID::Device::MouseE:        expansionPort = new MouseE; break;
    }
  }

  if(port == ID::Port::Arcade) {
    settings.arcadePanel = device;
    if(!system.loaded()) return;
  }

  cpu.peripherals.reset();
  cpu.peripherals.append(controllerPort1);
  cpu.peripherals.append(controllerPort2);
  cpu.peripherals.append(expansionPort);
}
