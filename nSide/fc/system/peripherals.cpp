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

auto Peripherals::connect(uint port, uint index) -> void {
  if(port == Port::Controller1) {
    settings.controllerPort1 = index;
    if(!system.loaded()) return;

    delete controllerPort1;
    switch(index) { default:
    case Peripheral::ControllerPort1::None:        controllerPort1 = new Controller(0, index); break;
    case Peripheral::ControllerPort1::Gamepad:     controllerPort1 = new Gamepad(0, index); break;
    case Peripheral::ControllerPort1::FourScore:   controllerPort1 = new FourScore(0, index); break;
    case Peripheral::ControllerPort1::SNESGamepad: controllerPort1 = new SNESGamepad(0, index); break;
    case Peripheral::ControllerPort1::Mouse:       controllerPort1 = new Mouse(0, index); break;
    }

    if(index == Peripheral::ControllerPort1::FourScore && settings.controllerPort2 != Peripheral::ControllerPort2::FourScore) {
      connect(Port::Controller2, Peripheral::ControllerPort2::FourScore);
    } else if(index != Peripheral::ControllerPort1::FourScore && settings.controllerPort2 == Peripheral::ControllerPort2::FourScore) {
      connect(Port::Controller2, Peripheral::ControllerPort2::None);
    }
  }

  if(port == Port::Controller2) {
    settings.controllerPort2 = index;
    if(!system.loaded()) return;

    delete controllerPort2;
    switch(index) { default:
    case Peripheral::ControllerPort2::None:        controllerPort2 = new Controller(1, index); break;
    case Peripheral::ControllerPort2::Gamepad:     controllerPort2 = new Gamepad(1, index); break;
    case Peripheral::ControllerPort2::FourScore:   controllerPort2 = new FourScore(1, index); break;
    case Peripheral::ControllerPort2::Zapper:      controllerPort2 = new Zapper(1, index); break;
    case Peripheral::ControllerPort2::PowerPad:    controllerPort2 = new PowerPad(1, index); break;
    case Peripheral::ControllerPort2::Vaus:        controllerPort2 = new Vaus(1, index); break;
    case Peripheral::ControllerPort2::SNESGamepad: controllerPort2 = new SNESGamepad(1, index); break;
    case Peripheral::ControllerPort2::Mouse:       controllerPort2 = new Mouse(1, index); break;
    }

    if(index == Peripheral::ControllerPort2::FourScore && settings.controllerPort1 != Peripheral::ControllerPort1::FourScore) {
      connect(Port::Controller1, Peripheral::ControllerPort1::FourScore);
    } else if(index != Peripheral::ControllerPort2::FourScore && settings.controllerPort1 == Peripheral::ControllerPort1::FourScore) {
      connect(Port::Controller1, Peripheral::ControllerPort1::None);
    }
  }

  if(port == Port::Expansion) {
    settings.expansionPort = index;
    if(!system.loaded()) return;

    delete expansionPort;
    switch(index) { default:
    case Peripheral::ExpansionPort::None:          expansionPort = new Expansion(index); break;
    case Peripheral::ExpansionPort::Gamepad:       expansionPort = new GamepadE(index); break;
    case Peripheral::ExpansionPort::FourPlayers:   expansionPort = new FourPlayers(index); break;
    case Peripheral::ExpansionPort::BeamGun:       expansionPort = new BeamGun(index); break;
    case Peripheral::ExpansionPort::FamilyTrainer: expansionPort = new FamilyTrainer(index); break;
    case Peripheral::ExpansionPort::Vaus:          expansionPort = new VausE(index); break;
    case Peripheral::ExpansionPort::SFCGamepad:    expansionPort = new SFCGamepad(index); break;
    case Peripheral::ExpansionPort::Mouse:         expansionPort = new MouseE(index); break;
    }
  }

  if(port == Port::Arcade) {
    settings.arcadePanel = index;
    if(!system.loaded()) return;
  }

  cpu.peripherals.reset();
  cpu.peripherals.append(controllerPort1);
  cpu.peripherals.append(controllerPort2);
  cpu.peripherals.append(expansionPort);
}
