struct Presentation : Window {
  Presentation();
  void showSplash();

  FixedLayout layout{this};
    Viewport viewport{&layout, Geometry{0, 0, 512, 480}};

  StatusBar statusBar{this};

  nall::image splash;
};

extern Presentation* presentation;
