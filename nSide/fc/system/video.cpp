auto System::configureVideoPalette() -> void {
  Emulator::video.setPalette();
}

auto System::configureVideoEffects() -> void {
  Emulator::video.setEffect(Emulator::Video::Effect::Scanlines, settings.scanlineEmulation);
  if(system.model() == Model::Famicom || system.model() == Model::FamicomBox) {
    Emulator::video.resize(256, 240);
  } else if(system.model() == Model::VSSystem) {
    Emulator::video.resize(256 * vssystem.gameCount, 240);
  } else if(system.model() == Model::PlayChoice10) {
    Emulator::video.resize(256, 240 + (playchoice10.screenConfig - 1) * 224);
  }
}
