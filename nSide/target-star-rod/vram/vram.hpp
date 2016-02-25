struct VRAMViewer : Window {
  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}, 5};
      Label modeLabel{&controlLayout, Size{0, 0}, 5};
      ComboButton modeSelection{&controlLayout, Size{~0, 0}, 5};
      Label paletteLabel{&controlLayout, Size{0, 0}, 5};
      ComboButton paletteSelection{&controlLayout, Size{~0, 0}, 5};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}, 5};
      Button update{&controlLayout, Size{80, 0}};
    Canvas canvas{&layout, Size{512, 512}};

  StatusBar statusBar{this};

  auto modeChanged() -> void;
  auto paletteChanged() -> void;
  auto updateTiles() -> void;
  VRAMViewer();
};

extern unique_pointer<VRAMViewer> vramViewer;
