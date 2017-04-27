#include "../shimai.hpp"
unique_pointer<Home> home;

#define maxVisibleCards 6
#define maxSelectableCards 4  //only used with looping list

Home::Home() {
  home = this;
  new Theme;
  new Graphics;
  new Sound;
  new Controls;

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

  theme->load(shimaiSettings["shimai/theme"].text());

  gameCursor = 0;
  gameCursorPosition = gameCursor;
  gameScroll = 0.0;
}

auto Home::reset() -> void {
  graphics->reset();
  sound->reset();
  controls->reset();

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

auto Home::loadSprites(vector<Sprite*>& sprites) -> void {
  double scale = graphics->buffer.height() / 240.0;

  menubarU.img = &theme->menubarU;
  menubarU.set(0.5, 0.0, 0.5, (int)(8 * scale));

  menubarL.img = &theme->menubarL;
  menubarL.set(0.5, 1.0, 0.5, menubarL.height() - (int)(10 * scale));

  captionTitle.set(0.5, 0.25, 0.5, 0.5, theme->captionTitle);
  captionTitleText.allocate(captionTitle.width(), captionTitle.height());
  captionTitleContents.set(0.5, 0.25, 0.5, 0.5, captionTitleText);

  gameCards.reset();
  for(uint i : range(min(home->games.size(), maxVisibleCards))) {
    gameCards.append(Sprite(0.5, 0.5, 0.5, 0.5, theme->gameCardBase));
  }

  cursorSprite.set(0.5, 0.5, 0.5, 0.5, theme->gameCursor);

  graphics->drawTextCenter(game().title, captionTitleText);

  sprites.append(&menubarU);
  sprites.append(&menubarL);
  sprites.append(&captionTitle);
  sprites.append(&captionTitleContents);
  for(auto& gameCard : gameCards) sprites.append(&gameCard);
  sprites.append(&cursorSprite);
};

auto Home::updateSprites() -> void {
  #define trueFmod(n, m)\
    fmod((fmod(n,m)+m),m)
  #define trueMod(n, m)\
    (n%m+m)%m

  uint gameCount = home->games.size();

  const double gameScrollRate = 1.0 / 16.0;
  double targetCursorPosition;
  if(gameCount >= maxVisibleCards) {
    //Why trueFmod is needed: When the first game is selected while the last
    //game is onscreen, gameScroll will be a high value, and gameCursor will
    //be 0, making the dividend a negative number.
    targetCursorPosition = trueFmod(gameCursor + 1.0 - gameScroll, gameCount) - 1.0;
    if(targetCursorPosition > maxSelectableCards - 1.0) {
      gameScroll = fmod(gameScroll + gameScrollRate, gameCount);
      gameCursorPosition -= gameScrollRate;
    } else if(targetCursorPosition < 0.0) {
      gameScroll = fmod(gameScroll + gameCount - gameScrollRate, gameCount);
      gameCursorPosition += gameScrollRate;
    } else if(gameScroll != round(gameScroll)) {
      gameScroll = round(gameScroll);
      gameCursorPosition = round(gameCursorPosition);
    }
  } else {
    targetCursorPosition = gameCursor;
  }
  if(gameCursorPosition < targetCursorPosition - gameScrollRate) {
    gameCursorPosition += gameScrollRate;
  } else if(gameCursorPosition > targetCursorPosition + gameScrollRate) {
    gameCursorPosition -= gameScrollRate;
  } else if(gameCursorPosition != targetCursorPosition) {
    gameCursorPosition = targetCursorPosition;
  }

  double cardX;
  double cursorX;
  if(gameCount >= maxVisibleCards) {
    for(int i : range(gameCards.size())) {
      auto& gameCard = gameCards[i];

      //Why trueMod is needed: When gameScroll is 0, the first 5 games will
      //be onscreen as well as the very last game at the far-left. "Very last"
      //is represented by -1 here.
      uint index = trueMod(i - 1 + (int)(gameScroll + 0.5), gameCount);
      cardX = (i + 0.5 - fmod(gameScroll + 0.5, 1.0)) / (maxVisibleCards - 1);
      gameCard.x = cardX * graphics->buffer.width();

      bool selected = cursorReady() && index == gameCursor;
      gameCard.img = selected ? &theme->gameCardActive : &theme->gameCards[index];
    }
    cursorX = (gameCursorPosition + 1.0) / (maxVisibleCards - 1);
  } else {
    for(int i : range(gameCards.size())) {
      auto& gameCard = gameCards[i];

      cardX = (i + (maxVisibleCards / 2.0) - gameCount * 0.5) / (maxVisibleCards - 1);
      gameCard.x = cardX * graphics->buffer.width();

      bool selected = cursorReady() && i == gameCursor;
      gameCard.img = selected ? &theme->gameCardActive : &theme->gameCards[i];
    }
    cursorX = (gameCursorPosition + (maxVisibleCards / 2.0) - gameCount * 0.5) / (maxVisibleCards - 1);
  }
  cursorSprite.x = cursorX * graphics->buffer.width();

  #undef trueFmod
  #undef trueMod
};

auto Home::setCursor(int cursor) -> void {
  gameCursor = cursor;
  theme->updateActiveGameCard();
  graphics->drawTextCenter(game().title, captionTitleText);
};

auto Home::cursorReady() -> bool {
  return fmod(home->gameScroll + home->gameCursorPosition, home->games.size()) == home->gameCursor;
};

auto Home::cursorLeft() -> void {
  if(cursorReady()) setCursor((gameCursor + games.size() - 1) % games.size());
};

auto Home::cursorRight() -> void {
  if(cursorReady()) setCursor((gameCursor + 1) % games.size());
};

auto Home::loadGame() -> void {
  if(cursorReady()) {
    program->mediumQueue.append(game().path());
    program->loadMedium();
    program->updateVideoShader();
  }
};

auto Home::Game::path() -> string {
  return {settings["Library/Location"].text(), home->system, "/", basename};
}

#undef maxVisibleCards
#undef maxSelectableCards
