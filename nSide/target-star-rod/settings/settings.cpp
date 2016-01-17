#include "../laevateinn.hpp"
Settings* settings = nullptr;

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

void Settings::load() {
  Configuration::Document::load(locate({localpath(), "star-rod/"}, "star-rod-settings.bml"));
  Configuration::Document::save(locate({localpath(), "star-rod/"}, "star-rod-settings.bml"));
}

void Settings::unload() {
  Configuration::Document::save(locate({localpath(), "star-rod/"}, "star-rod-settings.bml"));
}
