struct Theme {
  Theme();

  auto load(string themeName) -> void;
  auto loadBoxes() -> void;

  auto loadGameCard(uint gameID, bool active) -> image;
  auto updateActiveGameCard() -> void;

  string name;

  uint64 backgroundColor;

  image menubarU;
  image menubarL;
  image captionTitle;
  image gameCardBase;
  image gameCardActiveBase;
  image gameCursor;
  vector<image> boxes;
  vector<image> gameCards;
  image gameCardActive;

  struct Font {
    auto load() -> void;

    uint64 color;

    vector<shared_pointer<image>> glyphs;
    shared_pointer<image> u[1'114'112];
  } font;

  vfs::shared::file bgm;
};

extern unique_pointer<Theme> theme;
