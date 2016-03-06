struct BGViewer : Window {
  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}, 5};
      Label bgLabel{&controlLayout, Size{0, 0}, 5};
      ComboButton bgSelection{&controlLayout, Size{~0, 0}, 5};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}, 5};
      Button update{&controlLayout, Size{80, 0}};
    Canvas canvas{&layout, Size{512, 512}};

  StatusBar statusBar{this};

  auto bgChanged() -> void;
  auto updateTiles() -> void;
  BGViewer();
};

extern unique_pointer<BGViewer> bgViewer;
