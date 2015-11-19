#include "../base.hpp"
Settings *settings = nullptr;

void Settings::load() {
  config.append(folderpath, "folderpath");
  config.append(synchronizeAudio = true, "synchronizeAudio");
  config.append(muteAudio = false, "muteAudio");

  config.load({ program->userpath, "settings.cfg" });
  config.save({ program->userpath, "settings.cfg" });
}

void Settings::save() {
  config.save({ program->userpath, "settings.cfg" });
}
