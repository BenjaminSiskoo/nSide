#include "../tomoko.hpp"
Settings settings;

Settings::Settings() {
  Markup::Node::operator=(BML::unserialize(string::read(locate("settings.bml"))));

  auto set = [&](const string& name, const string& value) {
    //create node and set to default value only if it does not already exist
    if(!operator[](name)) operator()(name).setValue(value);
  };

  set("UserInterface/ShowStatusBar", true);

  set("Library/Location", {userpath(), "Emulation/"});
  set("Library/IgnoreManifests", false);

  set("Video/Driver", ruby::Video::optimalDriver());
  set("Video/Synchronize", false);
  set("Video/Scale", "Small");
  set("Video/AspectCorrection", true);
  set("Video/Shader", "Blur");
  set("Video/BlurEmulation", true);
  set("Video/ColorEmulation", true);
  set("Video/ScanlineEmulation", false);

  set("Video/Overscan/Mask", false);
  set("Video/Overscan/Horizontal", 8);
  set("Video/Overscan/Vertical", 8);

  set("Audio/Driver", ruby::Audio::optimalDriver());
  set("Audio/Device", "");
  set("Audio/Synchronize", true);
  set("Audio/Mute", false);
  set("Audio/Volume", 100);
  set("Audio/Latency", 60);
  set("Audio/Resampler", "Sinc");

  set("Input/Driver", ruby::Input::optimalDriver());
  set("Input/FocusLoss/Pause", false);
  set("Input/FocusLoss/AllowInput", false);
}

auto Settings::quit() -> void {
  file::write(locate("settings.bml"), BML::serialize(*this));
}
