auto System::configureVideoPalette() -> void {
  Emulator::video.setPalette();
}

auto System::configureVideoEffects() -> void {
  Emulator::video.setEffect(Emulator::Video::Effect::Scanlines, settings.scanlineEmulation);
  if(fc() || fcb()) {
    Emulator::video.resize(256, 240);
  } else if(vs()) {
    Emulator::video.resize(256 * vssystem.gameCount, 240);
  } else if(pc10()) {
    Emulator::video.resize(256, 240 + (playchoice10.screenConfig - 1) * 224);
  }
}
