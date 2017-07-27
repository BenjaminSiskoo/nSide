auto Program::message(const string &text) -> void {
  debugger->print(text, "\n");
}

auto Program::updateAudioEffects() -> void {
  auto volume = settings["Audio/Mute"].boolean() ? 0.0 : 1.0;
  Emulator::audio.setVolume(volume);
}
