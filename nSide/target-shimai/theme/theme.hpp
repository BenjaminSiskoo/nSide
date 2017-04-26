struct Theme {
  Theme();

  auto load(string themeName) -> void;
  auto loadBoxes() -> void;

  auto loadGameCard(uint gameID, bool active) -> image;
  auto updateActiveGameCard() -> void;

  string name;

  uint backgroundColor;

  image fontLatin;
  image fontKana;

  image menubarU;
  image menubarL;
  image captionTitle;
  image gameCardBase;
  image gameCardActiveBase;
  image gameCursor;
  vector<image> boxes;
  vector<image> gameCards;
  image gameCardActive;

  vfs::shared::file bgm;
};

extern unique_pointer<Theme> theme;
