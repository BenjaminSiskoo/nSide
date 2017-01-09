auto Program::powerCycle() -> void {
  if(!emulator) return;
  emulator->power();
  showMessage("Power cycled");
}

auto Program::softReset() -> void {
  if(!emulator) return;
  if(!emulator->information.resettable) return powerCycle();
  emulator->reset();
  showMessage("System reset");
}

auto Program::connectDevices() -> void {
  if(!emulator) return;
  for(auto& port : emulator->ports) {
    auto path = string{emulator->information.name, "/", port.name}.replace(" ", "");
    auto name = settings(path).text();
    for(auto& device : port.devices) {
      if(device.name == name) {
        emulator->connect(port.id, device.id);
        break;
      }
    }
  }
}

auto Program::showMessage(const string& text) -> void {
  statusTime = time(0);
  statusMessage = text;
}

auto Program::updateStatusText() -> void {
  time_t currentTime = time(0);

  string text;
  if((currentTime - statusTime) <= 2) {
    text = statusMessage;
  } else if(!emulator || emulator->loaded() == false) {
    text = "No cartridge loaded";
  } else if(pause || (!presentation->focused() && settings["Input/FocusLoss/Pause"].boolean())) {
    text = "Paused";
  } else {
    text = statusText;
  }

  if(text != presentation->statusBar.text()) {
    presentation->statusBar.setText(text);
  }
}

auto Program::updateVideoPalette() -> void {
  double saturation = settings["Video/Saturation"].natural() / 100.0;
  double gamma = settings["Video/Gamma"].natural() / 100.0;
  double luminance = settings["Video/Luminance"].natural() / 100.0;
  Emulator::video.setSaturation(saturation);
  Emulator::video.setGamma(gamma);
  Emulator::video.setLuminance(luminance);
  Emulator::video.setPalette();
}

auto Program::updateVideoShader() -> void {
  if(settings["Video/Driver"].text() == "OpenGL"
  && settings["Video/Shader"].text() != "None"
  && settings["Video/Shader"].text() != "Blur"
  && directory::exists(settings["Video/Shader"].text())
  ) {
    video->set(Video::Filter, Video::FilterNearest);
    video->set(Video::Shader, settings["Video/Shader"].text());
  } else {
    video->set(Video::Filter, settings["Video/Shader"].text() == "Blur" ? Video::FilterLinear : Video::FilterNearest);
    video->set(Video::Shader, (string)"");
  }
}

auto Program::updateAudioDriver() -> void {
  if(!audio) return;
  audio->clear();
  audio->set(Audio::Exclusive, settings["Audio/Exclusive"].boolean());
  audio->set(Audio::Latency, (uint)settings["Audio/Latency"].natural());
}

auto Program::updateAudioEffects() -> void {
  auto volume = settings["Audio/Mute"].boolean() ? 0.0 : settings["Audio/Volume"].natural() * 0.01;
  Emulator::audio.setVolume(volume);

  auto balance = max(-1.0, min(1.0, (settings["Audio/Balance"].integer() - 50) / 50.0));
  Emulator::audio.setBalance(balance);

  auto reverbEnable = settings["Audio/Reverb/Enable"].boolean();
  Emulator::audio.setReverb(reverbEnable);
}
