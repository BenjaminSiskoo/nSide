#include "../shimai.hpp"
#include "fc/interface/interface.hpp"
#include "graphics.cpp"
#include "sound.cpp"
#include "input.cpp"
#include "theme.cpp"
unique_pointer<Home> home;

Home::Home() {
  home = this;

  auto shimaiSettings = BML::unserialize(file::read(locate("shimai.bml")));

  console = shimaiSettings["shimai/console"].text();
  string type;
  for(auto& emulator : program->emulators) {
    if(emulator->information.name == console) {
      type = emulator->media[0].type;
      break;
    }
  }

  gameList.reset();
  string consolePath = {settings["Library/Location"].text(), console, "/"};
  gameList = directory::folders(consolePath, {"*.", type});
  for(uint index : rrange(gameList.size())) {
    if(!file::exists({gamePath(index), "box.png"})) gameList.remove(index);
  }
  //For testing the menu with a non-looping list (5 or fewer games)
  //gameList.resize(5);

  gameCursor = 0;

  graphics.initialize();

  updateTimer = 0;

  theme.load(shimaiSettings["shimai/theme"].text());
}

auto Home::reset() -> void {
  graphics.reset();
  sound.reset();
  inputManager->bind(famicom);
}

auto Home::run() -> void {
  sound.run();
  if(++updateTimer != audio->get(Audio::Frequency).get<uint>(44100) / 60) return;
  updateTimer = 0;
  graphics.run();
  if(!program->hasQuit) input.run();
}

auto Home::gamePath(uint index) -> string {
  return {settings["Library/Location"].text(), console, "/", gameList[index]};
}
