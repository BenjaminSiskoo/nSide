auto Home::Graphics::initialize() -> void {
  buffer.allocate(854, 480);

  gameCursorPosition = home->gameCursor;
  gameScroll = 0.0;
}

auto Home::Graphics::reset() -> void {
  video->set(Video::Filter, settings["Video/Shader"].text() == "None" ? Video::FilterNearest : Video::FilterLinear);
  video->set(Video::Shader, (string)"");
}

auto Home::Graphics::run() -> void {
  if(!video) return;

  auto& theme = home->theme;
  uint gameCount = home->gameList.size();

  const uint maxVisibleCards = 6;
  const uint maxSelectableCards = maxVisibleCards - 2;  //only when using looping list
  double scale = buffer.height() / 240.0;

  //TODO: These background colors are taken from the Famicom Mini and NES Classic
  //websites. Extract actual background color values from the encrypted Lua scripts.
  buffer.fill(theme.backgroundColor);

  #define trueFmod(n, m)\
    fmod((fmod(n,m)+m),m)
  #define trueMod(n, m)\
    (n%m+m)%m

  static auto drawAbsolute = [&](image img, int cx, int cy) -> void {
    uint x = max(0, cx);
    uint y = max(0, cy);
    if(x >= buffer.width() || y >= buffer.height()) return;
    uint rx = x - cx;
    uint ry = y - cy;
    if(rx >= img.width() || ry >= img.height()) return;
    uint rw = min(img.width () - rx, buffer.width () - x);
    uint rh = min(img.height() - ry, buffer.height() - y);
    buffer.impose(image::blend::sourceAlpha, x, y, img, rx, ry, rw, rh);
  };

  static auto drawCenter = [&](image img, double dx, double dy, double ox = 0.0, double oy = 0.0) -> void {
    int cx = dx * buffer.width () - ox * img.width ();
    int cy = dy * buffer.height() - oy * img.height();
    drawAbsolute(img, cx, cy);
  };

  const double gameScrollRate = 1.0 / 16.0;
  double targetCursorPosition;
  if(gameCount >= maxVisibleCards) {
    targetCursorPosition = trueFmod(home->gameCursor - gameScroll + 1.0, gameCount) - 1.0;
    if(targetCursorPosition > maxSelectableCards - 1.0) {
      gameScroll = trueFmod(gameScroll + gameScrollRate, gameCount);
      gameCursorPosition -= gameScrollRate;
    } else if(targetCursorPosition < 0.0) {
      gameScroll = trueFmod(gameScroll - gameScrollRate, gameCount);
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

  drawAbsolute(theme.menubarU, -4 * scale, -8 * scale);
  drawAbsolute(theme.menubarL, -4 * scale, buffer.height() - theme.menubarL.height() + 10 * scale);
  double cardX;
  double cursorX;
  if(gameCount >= maxVisibleCards) {
    for(int i : range(maxVisibleCards)) {
      uint index = trueMod(i - 1 + ((int)(gameScroll + 0.5) + (maxVisibleCards - 1) - i) / maxVisibleCards * maxVisibleCards, gameCount);
      cardX = (trueFmod((i + 0.5 - gameScroll), maxVisibleCards) - 0.5) / (maxVisibleCards - 1);
      image& card = cursorReady() && index == home->gameCursor ? theme.gameCardActive : theme.gameCards[index];
      drawCenter(card, cardX, 0.5, 0.5, 0.5);
    }
    cursorX = (gameCursorPosition + 1.0) / (maxVisibleCards - 1);
  } else {
    for(uint index : range(gameCount)) {
      image& card = cursorReady() && index == home->gameCursor ? theme.gameCardActive : theme.gameCards[index];
      cardX = (index + (maxVisibleCards / 2.0) - gameCount * 0.5) / (maxVisibleCards - 1);
      drawCenter(card, cardX, 0.5, 0.5, 0.5);
    }
    cursorX = (gameCursorPosition + (maxSelectableCards - 1) - 0.5 * gameCount) / (maxVisibleCards - 1);
  }
  drawCenter(theme.gameCursor, cursorX, 0.5, 0.5, 0.5);

  #undef trueFmod
  #undef trueMod

  auto output = (uint32*)buffer.data();
  uint width  = buffer.width();
  uint height = buffer.height();
  program->videoRefresh(output, width * sizeof(uint32), width, height);
}

auto Home::Graphics::cursorReady() -> bool {
  return fmod(gameScroll + gameCursorPosition, home->gameList.size()) == home->gameCursor;
}
