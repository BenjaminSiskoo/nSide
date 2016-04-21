auto System::configureVideoPalette() -> void {
  Emulator::video.setPalette();
}

auto System::configureVideoEffects() -> void {
  Emulator::video.setEffect(Emulator::Video::Effect::ColorBleed, settings.blurEmulation);
}

auto System::configureVideoCursors() -> void {
  Emulator::video.clearCursors();
  switch(settings.controllerPort2) {
  case Device::SuperScope:
    if(dynamic_cast<SuperScope*>(peripherals.controllerPort2)) {
      auto& controller = (SuperScope&)*peripherals.controllerPort2;
      Emulator::video.addCursor(controller.videoCursor);
    }
    break;
  case Device::Justifier:
  case Device::Justifiers:
    if(dynamic_cast<Justifier*>(peripherals.controllerPort2)) {
      auto& controller = (Justifier&)*peripherals.controllerPort2;
      Emulator::video.addCursor(controller.cursor1);
      if(!controller.chained) break;
      Emulator::video.addCursor(controller.cursor2);
    }
    break;
  }
}
