struct MemoryEditor : Window {
  VerticalLayout layout;
    HorizontalLayout controlLayout;
      Label gotoLabel;
      LineEdit gotoAddress;
      ComboButton source;
      Button exportMemory;
      Widget spacer;
      CheckLabel autoUpdate;
      Button update;
    HexEdit editor;

  uint8 read(uint addr);
  void write(uint addr, uint8 data);
  void selectSource();
  void exportMemoryToDisk();
  void updateView();
  MemoryEditor();
};

extern MemoryEditor* memoryEditor;
