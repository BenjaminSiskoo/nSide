struct VRAMViewer : Window {
  VerticalLayout layout;
    HorizontalLayout controlLayout;
      Label modeLabel;
      ComboButton modeSelection;
      Widget spacer;
      CheckLabel autoUpdate;
      Button update;
    Canvas canvas;

  void updateTiles();
  VRAMViewer();
};

extern VRAMViewer *vramViewer;
