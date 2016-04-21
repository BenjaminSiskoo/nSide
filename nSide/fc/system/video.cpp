auto System::configureVideoPalette() -> void {
  Emulator::video.setPalette();
}

auto System::configureVideoEffects() -> void {
  Emulator::video.setEffect(Emulator::Video::Effect::Scanlines, settings.scanlineEmulation);
  Emulator::video.resize(interface->information.canvasWidth, interface->information.canvasHeight);
}

auto System::configureVideoCursors() -> void {
  Emulator::video.clearCursors();
  if(settings.expansionPort == Device::BeamGun) { //Beam Gun
    if(dynamic_cast<BeamGun*>(peripherals.expansionPort)) {
      auto& controller = (BeamGun&)*peripherals.expansionPort;
      Emulator::video.addCursor(controller.cursor);
    }
  }
  if(settings.controllerPort2 == Device::BeamGun) { //Zapper
    if(dynamic_cast<BeamGun*>(peripherals.controllerPort2)) {
      auto& controller = (BeamGun&)*peripherals.controllerPort2;
      Emulator::video.addCursor(controller.cursor);
    }
  }
}
