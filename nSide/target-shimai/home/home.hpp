struct Home {
  Home();

  auto reset() -> void;
  auto run() -> void;

  auto gamePath(uint index) -> string;

  //video.cpp
  struct Graphics {
    auto initialize() -> void;
    auto reset() -> void;
    auto run() -> void;

    auto cursorReady() -> bool;

    image buffer;
    double gameCursorPosition;
    double gameScroll;
  } graphics;

  //audio.cpp
  struct Sound {
    auto reset() -> void;
    auto run() -> void;

    shared_pointer<Emulator::Stream> stream;
    vfs::shared::file audioFile;
    uint playOffset = 0;
    uint loopOffset;
  } sound;

  //input.cpp
  struct Input {
    auto reset() -> void;
    auto run() -> void;
    auto poll() -> void;

    bool up;
    bool down;
    bool left;
    bool right;
    bool b;
    bool a;
    bool select;
    bool start;
    bool previousUp;
    bool previousDown;
    bool previousLeft;
    bool previousRight;
    bool previousB;
    bool previousA;
    bool previousSelect;
    bool previousStart;
  } input;

  //theme.cpp
  struct Theme {
    auto load(string themeName) -> void;
    auto loadBoxes() -> void;

    auto loadGameCard(uint gameID, bool active) -> image;
    auto updateActiveGameCard() -> void;

    string name;

    uint backgroundColor;

    image menubarU;
    image menubarL;
    image gameCardBase;
    image gameCardActiveBase;
    image gameCursor;
    vector<image> boxes;
    vector<image> gameCards;
    image gameCardActive;
  } theme;
  string console;
  string_vector gameList;
  uint gameCursor;

  uint updateTimer;

  Emulator::Interface* famicom;

};

extern unique_pointer<Home> home;
