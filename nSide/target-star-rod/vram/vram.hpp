struct VRAMViewer : Window {
  VRAMViewer();

  auto modeChanged() -> void;
  auto paletteChanged() -> void;
  auto columnChanged() -> void;
  auto updateTiles() -> void;

  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Label modeLabel{&controlLayout, Size{0, 0}};
      ComboButton modeSelection{&controlLayout, Size{~0, 0}};
      Label paletteLabel{&controlLayout, Size{0, 0}};
      ComboButton paletteSelection{&controlLayout, Size{~0, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}};
      Button update{&controlLayout, Size{80, 0}};
    HorizontalLayout columnLayout{&layout, Size{~0, 0}};
      RadioLabel columnLinear{&columnLayout, Size{100, 0}};
      RadioLabel column16{&columnLayout, Size{100, 0}};
      Group columnGroup{&columnLinear, &column16};
    Canvas canvas{&layout, Size{512, 512}};

  StatusBar statusBar{this};
};

extern unique_pointer<VRAMViewer> vramViewer;
