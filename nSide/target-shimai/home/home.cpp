#include "../shimai.hpp"
unique_pointer<Home> home;

Home::Home() {
  home = this;
  new Graphics;
  new Sound;
  new Controls;
  new Theme;

  auto shimaiSettings = BML::unserialize(file::read(locate("shimai.bml")));

  system = shimaiSettings["shimai/system"].text();
  string type;
  for(auto& emulator : program->emulators) {
    if(emulator->information.name == system) {
      type = emulator->media[0].type;
      break;
    }
  }

  games.reset();
  string consolePath = {settings["Library/Location"].text(), system, "/"};
  auto gameList = directory::folders(consolePath, {"*.", type});
  for(string basename : gameList) {
    Game game;
    game.basename = basename;
    if(!file::exists({game.path(), "box.png"})) continue;

    string markup;
    string manifestPath = {game.path(), "manifest.bml"};
    if(!file::exists(manifestPath) || settings["Library/IgnoreManifests"].boolean()) {
      if(auto manifest = execute("cart-pal", "--manifest", game.path())) {
        markup = manifest.output;
      }
    } else {
      markup = file::read(manifestPath);
    }
    auto manifest = BML::unserialize(markup);
    game.name = manifest["information/name"] ? manifest["information/name"].text() : basename;
    game.title = manifest["information/title"] ? manifest["information/title"].text() : basename;

    games.append(game);
  }

  graphics->initialize();

  gameCursor = 0;

  theme->load(shimaiSettings["shimai/theme"].text());
}

auto Home::reset() -> void {
  graphics->reset();
  sound->reset();
  controls->reset();

  graphics->drawCaptionText();

  controls->onPressLeft  = {&cursorLeft,  this};
  controls->onPressRight = {&cursorRight, this};
  controls->onPressA     = {&loadGame, this};
  controls->onPressStart = {&loadGame, this};
}

auto Home::run() -> void {
  graphics->run();
  sound->run();
  if(graphics->step == 0 && !program->hasQuit) controls->run();
}

auto Home::game() -> Game& {
  return games[gameCursor];
}

auto Home::setCursor(int cursor) -> void {
  gameCursor = cursor;
  theme->updateActiveGameCard();
  graphics->drawCaptionText();
};

auto Home::cursorLeft() -> void {
  if(graphics->cursorReady()) setCursor((gameCursor + games.size() - 1) % games.size());
};

auto Home::cursorRight() -> void {
  if(graphics->cursorReady()) setCursor((gameCursor + 1) % games.size());
};

auto Home::loadGame() -> void {
  if(graphics->cursorReady()) {
    program->mediumQueue.append(game().path());
    program->loadMedium();
    program->updateVideoShader();
  }
};

auto Home::Game::path() -> string {
  return {settings["Library/Location"].text(), home->system, "/", basename};
}
