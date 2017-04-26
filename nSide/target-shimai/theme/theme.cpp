#include "../shimai.hpp"
unique_pointer<Theme> theme;

Theme::Theme() {
  theme = this;
}

auto Theme::load(string themeName) -> void {
  name = themeName;

  string path = {"Themes/", name, "/"};
  auto manifest = BML::unserialize(file::read(locate({path, "manifest.bml"})));

  backgroundColor = 0xff000000 | manifest["theme/background-color"].natural();

  double scale = graphics->buffer.height() / 240.0;
  uint w,h;

  image texture;
  if(file::exists(locate({path, "graphics.png"}))) texture.load(locate({path, "graphics.png"}));

  static auto extract = [&](string name, image& img, bool doScale) -> image {
    auto spriteNode = manifest[{"theme/sprites/", name}];
    if(texture && spriteNode) {
      uint x = spriteNode["x"].natural();
      uint y = spriteNode["y"].natural();
      uint w = spriteNode["w"].natural();
      uint h = spriteNode["h"].natural();
      img.allocate(w, h);
      img.impose(image::blend::sourceColor, 0, 0, texture, x, y, w, h);
    } else {
      img.load(locate({path, name, ".png"}));
    }
    if(doScale) img.scale(img.width() * scale, img.height() * scale, false);
    return img;
  };

  extract("fontLatin", fontLatin, true);
  extract("fontKana",  fontKana,  true);

  extract("menubarU", menubarU, true);
  extract("menubarL", menubarL, true);
  extract("captionTitle", captionTitle, true);
  extract("gameCard", gameCardBase, true);
  extract("gameCardActive", gameCardActiveBase, true);

  gameCursor.allocate(gameCardBase.width() + 8 * scale, gameCardBase.height() + 8 * scale);
  gameCursor.fill(0x00000000);
  image cursorPart;
  static auto imposeCursorPart = [&](string id, uint x, uint y) -> void {
    extract({"cursorGame", id}, cursorPart, false);
    cursorPart.crop(1, 1, 30, 30);
    cursorPart.scale(w, h, false);
    gameCursor.impose(image::blend::sourceAlpha, x, y, cursorPart, 0, 0, w, h);
  };
  w = 10 * scale;
  h = 10 * scale;
  imposeCursorPart("1", 0,                      0);
  imposeCursorPart("3", gameCursor.width() - w, 0);
  imposeCursorPart("7", 0,                      gameCursor.height() - h);
  imposeCursorPart("9", gameCursor.width() - w, gameCursor.height() - h);
  w = gameCursor.width() - 16 * scale;
  h = 10 * scale;
  imposeCursorPart("2", 8 * scale, 0);
  imposeCursorPart("8", 8 * scale, gameCursor.height() - h);
  w = 10 * scale;
  h = gameCursor.height() - 16 * scale;
  imposeCursorPart("4", 0,                      8 * scale);
  imposeCursorPart("6", gameCursor.width() - w, 8 * scale);
  gameCursor.crop(scale * 2, scale * 2, gameCursor.width() - scale * 2, gameCursor.height() - scale * 2);

  loadBoxes();
  gameCards.reset();
  for(uint id : range(home->games.size())) gameCards.append(loadGameCard(id, false));
  updateActiveGameCard();

  bool audioValid = false;
  if(bgm = vfs::fs::file::open(locate({path, "bgm.pcm"}), vfs::file::mode::read)) {
    if(bgm->size() >= 8) {
      uint32 header = bgm->readm(4);
      if(header == 0x4d535531) {  //"MSU1"
        audioValid = true;
      }
    }
  }
  if(!audioValid) bgm.reset();
}

auto Theme::loadBoxes() -> void {
  boxes.reset();
  for(uint gameID : range(home->games.size())) {
    image& box = boxes(gameID);
    box.load(string{home->games[gameID].path(), "box.png"});

    double scale = graphics->buffer.height() / 240.0;

    uint boundX = 2 * scale;
    uint boundY = 2 * scale;
    uint boundW = gameCardBase.width () - boundX -  4 * scale;
    uint boundH = gameCardBase.height() - boundY - 20 * scale;

    uint rectW = boundW;
    uint rectH = boundH;
    if(box.width() > box.height() * boundW / boundH) {
      rectH = box.height() * boundW / box.width();
    } else {
      rectW = box.width() * boundH / box.height();
    }
    box.scale(rectW, rectH, true);
  }
}

auto Theme::loadGameCard(uint gameID, bool active) -> image {
  image card{active ? gameCardActiveBase : gameCardBase};

  image& box = boxes[gameID];
  if(box) {
    double scale = graphics->buffer.height() / 240.0;

    uint boundX = 2 * scale;
    uint boundY = 2 * scale;
    uint boundW = card.width () - boundX -  4 * scale;
    uint boundH = card.height() - boundY - 20 * scale;

    uint rectX = boundX + boundW / 2 - box.width () / 2;
    uint rectY = boundY + boundH / 2 - box.height() / 2;
    card.impose(image::blend::sourceColor, rectX, rectY, box, 0, 0, box.width(), box.height());
  }

  return card;
}

auto Theme::updateActiveGameCard() -> void {
  gameCardActive = loadGameCard(home->gameCursor, true);
}
