struct MemoryEditor : Window {
  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}, 5};
      Label gotoLabel{&controlLayout, Size{0, 0}, 5};
      LineEdit gotoAddress{&controlLayout, Size{50, 0}, 5};
      ComboButton source{&controlLayout, Size{0, 0}, 5};
      Button exportMemory{&controlLayout, Size{80, 0}, 5};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}, 5};
      Button update{&controlLayout, Size{80, 0}};
    HexEdit editor{&layout, Size{~0, ~0}};

  enum : uint { CPU, APU, VRAM, OAM, CGRAM };

  auto read(uint addr) -> uint8_t;
  auto write(uint addr, uint8_t data) -> void;
  auto selectSource() -> void;
  auto exportMemoryToDisk() -> void;
  auto updateView() -> void;
  MemoryEditor();
};

extern unique_pointer<MemoryEditor> memoryEditor;
