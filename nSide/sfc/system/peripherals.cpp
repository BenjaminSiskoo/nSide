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
}

auto Peripherals::connect(uint port, uint index) -> void {
  if(port == Port::Controller1) {
    settings.controllerPort1 = index;
    if(!system.loaded()) return;

    delete controllerPort1;
    switch(index) { default:
    case Peripheral::ControllerPort1::None:     controllerPort1 = new Controller(0, index); break;
    case Peripheral::ControllerPort1::Gamepad:  controllerPort1 = new Gamepad(0, index); break;
    case Peripheral::ControllerPort1::Multitap: controllerPort1 = new Multitap(0, index); break;
    case Peripheral::ControllerPort1::Mouse:    controllerPort1 = new Mouse(0, index); break;
    }
  }

  if(port == Port::Controller2) {
    settings.controllerPort2 = index;
    if(!system.loaded()) return;

    delete controllerPort2;
    switch(index) { default:
    case Peripheral::ControllerPort2::None:       controllerPort2 = new Controller(1, index); break;
    case Peripheral::ControllerPort2::Gamepad:    controllerPort2 = new Gamepad(1, index); break;
    case Peripheral::ControllerPort2::Multitap:   controllerPort2 = new Multitap(1, index); break;
    case Peripheral::ControllerPort2::Mouse:      controllerPort2 = new Mouse(1, index); break;
    case Peripheral::ControllerPort2::SuperScope: controllerPort2 = new SuperScope(1, index); break;
    case Peripheral::ControllerPort2::Justifier:  controllerPort2 = new Justifier(1, index, false); break;
    case Peripheral::ControllerPort2::Justifiers: controllerPort2 = new Justifier(1, index, true); break;
    }
  }

  if(port == Port::Expansion) {
    settings.expansionPort = index;
    if(!system.loaded()) return;

    delete expansionPort;
    switch(index) { default:
    case Peripheral::ExpansionPort::None:        expansionPort = new Expansion; break;
    case Peripheral::ExpansionPort::Satellaview: expansionPort = new Satellaview; break;
    case Peripheral::ExpansionPort::SuperDisc:   expansionPort = new SuperDisc; break;
    case Peripheral::ExpansionPort::S21FX:       expansionPort = new S21FX; break;
    }
  }

  cpu.peripherals.reset();
  cpu.peripherals.append(controllerPort1);
  cpu.peripherals.append(controllerPort2);
  cpu.peripherals.append(expansionPort);
}
