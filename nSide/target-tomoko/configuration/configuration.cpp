#include "../tomoko.hpp"
Settings settings;

Settings::Settings() {
  Markup::Node::operator=(BML::unserialize(string::read(locate("settings.bml"))));

  auto set = [&](const string& name, const string& value) {
    //create node and set to default value only if it does not already exist
    if(!operator[](name)) operator()(name).setValue(value);
  };

  set("UserInterface/ShowStatusBar", true);

  set("Library/Location", {Path::user(), "Emulation/"});
  set("Library/IgnoreManifests", false);

  set("Video/Driver", ruby::Video::optimalDriver());
  set("Video/Synchronize", false);
  set("Video/Shader", "Blur");
  set("Video/BlurEmulation", true);
  set("Video/ColorEmulation", true);
  set("Video/ScanlineEmulation", false);

  set("Video/Saturation", 100);
  set("Video/Gamma", 100);
  set("Video/Luminance", 100);

  set("Video/Overscan/Mask", false);
  set("Video/Overscan/Horizontal", 8);
  set("Video/Overscan/Vertical", 8);

  set("Video/Windowed/AspectCorrection", true);
  set("Video/Windowed/Adaptive", false);
  set("Video/Windowed/Multiplier", "Small");
  set("Video/Windowed/Multiplier/Small", 2);
  set("Video/Windowed/Multiplier/Medium", 3);
  set("Video/Windowed/Multiplier/Large", 4);

  set("Video/Fullscreen/AspectCorrection", true);
  set("Video/Fullscreen/Adaptive", false);

  set("Audio/Driver", ruby::Audio::optimalDriver());
  set("Audio/Device", "");
  set("Audio/Exclusive", false);
  set("Audio/Synchronize", true);
  set("Audio/Mute", false);
  set("Audio/Volume", 100);
  set("Audio/Balance", 50);
  set("Audio/Reverb/Enable", false);
  set("Audio/Latency", 60);
  set("Audio/Resampler", "Sinc");

  set("Input/Driver", ruby::Input::optimalDriver());
  set("Input/Frequency", 5);
  set("Input/FocusLoss/Pause", false);
  set("Input/FocusLoss/AllowInput", false);
}

auto Settings::quit() -> void {
  file::write(locate("settings.bml"), BML::serialize(*this));
}
