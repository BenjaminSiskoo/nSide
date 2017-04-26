#include "../shimai.hpp"
#include "sprite.cpp"
unique_pointer<Graphics> graphics;

#define maxVisibleCards 6
#define maxSelectableCards 4  //only used with looping list

Graphics::Graphics() {
  graphics = this;
};

auto Graphics::initialize() -> void {
  buffer.allocate(854, 480);

  gameCursorPosition = home->gameCursor;
  gameScroll = 0.0;
}

auto Graphics::reset() -> void {
  double scale = buffer.height() / 240.0;

  video->set(Video::Filter, settings["Video/Shader"].text() == "None" ? Video::FilterNearest : Video::FilterLinear);
  video->set(Video::Shader, (string)"");
  step = 0;

  sprites.reset();

  menubarU.img = &theme->menubarU;
  menubarU.set(0.5, 0.0, 0.5, (int)(8 * scale));

  menubarL.img = &theme->menubarL;
  menubarL.set(0.5, 1.0, 0.5, menubarL.height() - (int)(10 * scale));

  captionTitle.set(0.5, 0.25, 0.5, 0.5, theme->captionTitle);
  captionTitleText.allocate(captionTitle.width(), captionTitle.height());
  captionTitleContents.set(0.5, 0.25, 0.5, 0.5, captionTitleText);

  for(uint i : range(min(home->games.size(), maxVisibleCards))) {
    gameCards.append(Sprite(0.5, 0.5, 0.5, 0.5, theme->gameCardBase));
  }

  gameCursor.set(0.5, 0.5, 0.5, 0.5, theme->gameCursor);

  sprites.append(&menubarU);
  sprites.append(&menubarL);
  sprites.append(&captionTitle);
  sprites.append(&captionTitleContents);
  for(auto& gameCard : gameCards) sprites.append(&gameCard);
  sprites.append(&gameCursor);
}

auto Graphics::run() -> void {
  double scale = buffer.height() / 240.0;
  uint gameCount = home->games.size();

  #define trueFmod(n, m)\
    fmod((fmod(n,m)+m),m)
  #define trueMod(n, m)\
    (n%m+m)%m

  uint sampleRate = audio->get(Audio::Frequency).get<uint>(44100) / 60;

  if(step == 0) {
    const double gameScrollRate = 1.0 / 16.0;
    double targetCursorPosition;
    if(gameCount >= maxVisibleCards) {
      //Why trueFmod is needed: When the first game is selected while the last
      //game is onscreen, gameScroll will be a high value, and gameCursor will
      //be 0, making the dividend a negative number.
      targetCursorPosition = trueFmod(home->gameCursor + 1.0 - gameScroll, gameCount) - 1.0;
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
      targetCursorPosition = home->gameCursor;
    }
    if(gameCursorPosition < targetCursorPosition - gameScrollRate) {
      gameCursorPosition += gameScrollRate;
    } else if(gameCursorPosition > targetCursorPosition + gameScrollRate) {
      gameCursorPosition -= gameScrollRate;
    } else if(gameCursorPosition != targetCursorPosition) {
      gameCursorPosition = targetCursorPosition;
    }
  }
  
  if(step == 1) {
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
        gameCard.x = cardX * buffer.width();

        bool selected = cursorReady() && index == home->gameCursor;
        gameCard.img = selected ? &theme->gameCardActive : &theme->gameCards[index];
      }
      cursorX = (gameCursorPosition + 1.0) / (maxVisibleCards - 1);
    } else {
      for(int i : range(gameCards.size())) {
        auto& gameCard = gameCards[i];

        cardX = (i + (maxVisibleCards / 2.0) - gameCount * 0.5) / (maxVisibleCards - 1);
        gameCard.x = cardX * buffer.width();

        bool selected = cursorReady() && i == home->gameCursor;
        gameCard.img = selected ? &theme->gameCardActive : &theme->gameCards[i];
      }
      cursorX = (gameCursorPosition + (maxSelectableCards - 1) - 0.5 * gameCount) / (maxVisibleCards - 1);
    }
    gameCursor.x = cursorX * buffer.width();
  }

  if(step == 2) {
    //TODO: These background colors are taken from the Famicom Mini and NES Classic
    //websites. Extract actual background color values from the encrypted Lua scripts.
    buffer.fill(theme->backgroundColor);
  }

  if(step >= 3 && step < sprites.size() + 3) {
    auto& sprite = *sprites[step - 3];
    uint x = max(0, sprite.x - sprite.ox);
    uint y = max(0, sprite.y - sprite.oy);
    if(x < buffer.width() && y < buffer.height()) {
      uint rx = x - (sprite.x - sprite.ox);
      uint ry = y - (sprite.y - sprite.oy);
      if(rx < sprite.img->width() && ry < sprite.img->height()) {
        uint rw = min(sprite.img->width () - rx, buffer.width () - x);
        uint rh = min(sprite.img->height() - ry, buffer.height() - y);
        buffer.impose(image::blend::sourceAlpha, x, y, *sprite.img, rx, ry, rw, rh);
      }
    }
  }

  if(step == sampleRate - 1) {
    auto output = (uint32*)buffer.data();
    uint width  = buffer.width();
    uint height = buffer.height();
    program->videoRefresh(output, width * sizeof(uint32), width, height);
  }

  #undef trueFmod
  #undef trueMod

  if(step++ >= sampleRate) step = 0;
}

auto Graphics::drawCaptionText() -> void {
  captionTitleText.fill(0x00000000);
  uint w = theme->fontLatin.width() / 16;
  uint h = theme->fontLatin.height() / 16;

  string title = home->game().title;

  vector<uint32_t> codepoints;
  uint32_t codepoint;
  uint remainingBytes = 0;
  uint byteIndex = 0;
  while(byteIndex < title.size()) {
    if(remainingBytes) {
      remainingBytes--;
      codepoint |= (title[byteIndex++] & 0x3f) << (remainingBytes * 6);
      if(remainingBytes) continue;
      codepoints.append(codepoint);
      codepoint = 0;
    } else if(title[byteIndex] & 0x80) {
      if((title[byteIndex] & 0xe0) == 0xc0) remainingBytes = 1;
      if((title[byteIndex] & 0xf0) == 0xe0) remainingBytes = 2;
      if((title[byteIndex] & 0xf8) == 0xf0) remainingBytes = 3;
      codepoint = (title[byteIndex++] & (0x3f >> remainingBytes)) << (remainingBytes * 6);
    } else {
      codepoints.append(title[byteIndex++]);
    }
  }

  uint startX = (captionTitleText.width() - codepoints.size() * w) / 2;
  uint startY = (captionTitleText.height() - h) / 2;
  for(uint loc : range(codepoints)) {
    uint32_t codepoint = codepoints[loc];
    auto& font = codepoint >> 8 == 0x30 ? theme->fontKana : theme->fontLatin;
    if(codepoint >> 8 == 0xff) codepoint -= 0xfee0;
    uint x = startX + loc * w;
    uint cx = (codepoint >> 0 & 0x0f) * w;
    uint cy = (codepoint >> 4 & 0x0f) * h;
    captionTitleText.impose(image::blend::sourceAlpha, x, startY, font, cx, cy, w, h);
  }
}

auto Graphics::cursorReady() -> bool {
  return fmod(gameScroll + gameCursorPosition, home->games.size()) == home->gameCursor;
}

#undef maxVisibleCards
#undef maxSelectableCards
