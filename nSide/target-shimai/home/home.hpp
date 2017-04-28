struct Home {
  Home();

  struct Game {
    auto path() -> string;

    string basename;
    string name;
    string title;
  };
  vector<Game> games;

  auto reset() -> void;
  auto run() -> void;

  auto game() -> Game&;

  auto loadSprites(vector<Sprite*>& sprites) -> void;
  auto updateSprites() -> void;

  auto cursorLeft() -> void;
  auto cursorRight() -> void;
  auto loadGame() -> void;

  string system;

private:
  auto setCursor(int cursor) -> void;
  auto cursorReady() -> bool;

  uint gameCursor;
  double gameCursorPosition;
  double gameScroll;

  Sprite menubarU;
  Sprite menubarL;
  Sprite captionTitle;
  Sprite captionTitleContents;
  image captionTitleText;
  vector<Sprite> gameCards;
  Sprite cursorSprite;
};

extern unique_pointer<Home> home;
