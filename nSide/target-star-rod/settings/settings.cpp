#include "../laevateinn.hpp"
Settings* settings = nullptr;

Settings::Settings() {
  settings = this;

  video.append(video.driver = ruby::Video::safestDriver(), "Driver");
  video.append(video.synchronize = false, "Synchronize");
  append(video, "Video");

  audio.append(audio.driver = ruby::Audio::safestDriver(), "Driver");
  audio.append(audio.synchronize = true, "Synchronize");
  audio.append(audio.mute = false, "Mute");
  append(audio, "Audio");

  input.append(input.driver = ruby::Input::safestDriver(), "Driver");
  append(input, "Input");

  load();
}

void Settings::load() {
  Configuration::Document::load(program->path("star-rod-settings.bml"));
  Configuration::Document::save(program->path("star-rod-settings.bml"));
}

void Settings::unload() {
  Configuration::Document::save(program->path("star-rod-settings.bml"));
}
