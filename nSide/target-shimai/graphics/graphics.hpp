struct Graphics {
  Graphics();

  auto initialize() -> void;
  auto reset() -> void;
  auto run() -> void;

  auto drawCaptionText() -> void;

  auto cursorReady() -> bool;

  image buffer;
  uint step;

  double gameCursorPosition;
  double gameScroll;

  struct Sprite {
    Sprite();
    Sprite(any x, any y, any ox, any oy, image&);

    auto set(any, any, any, any) -> void;
    auto set(any, any, any, any, image&) -> void;

    auto width() -> uint;
    auto height() -> uint;

    int x;
    int y;
    int ox;
    int oy;
    image* img;
  };

  Sprite menubarU;
  Sprite menubarL;
  Sprite captionTitle;
  Sprite captionTitleContents;
  image captionTitleText;
  vector<Sprite> gameCards;
  Sprite gameCursor;

  vector<Sprite*> sprites;
};

extern unique_pointer<Graphics> graphics;
