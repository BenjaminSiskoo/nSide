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

  auto setCursor(int cursor) -> void;

  auto cursorLeft() -> void;
  auto cursorRight() -> void;
  auto loadGame() -> void;

  string system;
  uint gameCursor;
};

extern unique_pointer<Home> home;
