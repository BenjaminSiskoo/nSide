#include "../laevateinn.hpp"
unique_pointer<Settings> settings;

Settings::Settings() {
  settings = this;

  video.append(video.driver = ruby::Video::optimalDriver(), "Driver");
  video.append(video.synchronize = false, "Synchronize");
  append(video, "Video");

  audio.append(audio.driver = ruby::Audio::optimalDriver(), "Driver");
  audio.append(audio.synchronize = true, "Synchronize");
  audio.append(audio.mute = false, "Mute");
  append(audio, "Audio");

  input.append(input.driver = ruby::Input::optimalDriver(), "Driver");
  append(input, "Input");

  load();
}

auto Settings::load() -> void {
  Configuration::Document::load(locate("settings.bml"));
  Configuration::Document::save(locate("settings.bml"));
}

auto Settings::unload() -> void {
  Configuration::Document::save(locate("settings.bml"));
}
