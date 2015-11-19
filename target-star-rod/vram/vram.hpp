struct VRAMViewer : Window {
  VerticalLayout layout;
    HorizontalLayout controlLayout;
      Label modeLabel;
      ComboButton modeSelection;
      Label paletteLabel;
      ComboButton paletteSelection;
      Widget spacer;
      CheckLabel autoUpdate;
      Button update;
    Canvas canvas;

  void modeChanged();
  void paletteChanged();
  void updateTiles();
  VRAMViewer();
};

extern VRAMViewer* vramViewer;
