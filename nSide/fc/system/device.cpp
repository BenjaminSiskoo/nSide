Device device;

Device::Device() {
  connect(0, ID::None);
  connect(1, ID::None);
  connect(2, ID::None);
  connect(3, ID::None);
}

Device::~Device() {
  if(controllerPort1) delete controllerPort1;
  if(controllerPort2) delete controllerPort2;
  if(expansionPort) delete expansionPort;
}

auto Device::connect(uint port, Device::ID id) -> void {
  if(port == 0 || port == 1 || port == 2) {
    Controller*& controller = (
      port == 0 ? controllerPort1 :
      port == 1 ? controllerPort2 :
                  expansionPort
    );

    if(controller) {
      delete controller;
      controller = nullptr;
    }

    switch(id) { default:
    case ID::None: controller = new Controller(port); break;
    case ID::Gamepad: controller = new Gamepad(port); break;
    case ID::FourPlayers: controller = new FourPlayers(port); break;
    case ID::FourScore: controller = new FourScore(port); break;
    case ID::BeamGun: controller = new BeamGun(port); break;
    case ID::FamilyTrainer: controller = new FamilyTrainer(port); break;
    case ID::Vaus: controller = new Vaus(port); break;
    case ID::SFCGamepad: controller = new SFCGamepad(port); break;
    case ID::Mouse: controller = new Mouse(port); break;
    }

    switch(port) {
    case 0: settings.controllerPort1 = (uint)id; break;
    case 1: settings.controllerPort2 = (uint)id; break;
    case 2: settings.expansionPort   = (uint)id; break;
    }

    switch((Port)port) {
    case Port::Controller1:
      if(id == ID::FourScore && settings.controllerPort2 != (uint)ID::FourScore) {
        connect((uint)Port::Controller2, ID::FourScore);
        interface->deviceChanged((uint)Port::Controller2, (uint)ID::FourScore);
      } else if(id != ID::FourScore && settings.controllerPort2 == (uint)ID::FourScore) {
        connect((uint)Port::Controller2, ID::None);
        interface->deviceChanged((uint)Port::Controller2, (uint)ID::None);
      }
      break;
    case Port::Controller2:
      if(id == ID::FourScore && settings.controllerPort1 != (uint)ID::FourScore) {
        connect((uint)Port::Controller1, ID::FourScore);
        interface->deviceChanged((uint)Port::Controller1, (uint)ID::FourScore);
      } else if(id != ID::FourScore && settings.controllerPort1 == (uint)ID::FourScore) {
        connect((uint)Port::Controller1, ID::None);
        interface->deviceChanged((uint)Port::Controller1, (uint)ID::None);
      }
      break;
    }
  }

  if(port == 3) {
    settings.arcadePanel = (uint)id;
  }

  cpu.peripherals.reset();
  cpu.peripherals.append(controllerPort1);
  cpu.peripherals.append(controllerPort2);
  cpu.peripherals.append(expansionPort);
}
