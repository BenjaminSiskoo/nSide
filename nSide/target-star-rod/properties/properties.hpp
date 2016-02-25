struct PropertiesViewer : Window {
  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}, 5};
      Label sourceLabel{&controlLayout, Size{0, 0}, 5};
      ComboButton sourceSelection{&controlLayout, Size{0, 0}, 5};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}, 5};
      Button update{&controlLayout, Size{0, 0}};
    TextEdit properties{&layout, Size{~0, ~0}};

  auto updateProperties() -> void;
  PropertiesViewer();
};

extern unique_pointer<PropertiesViewer> propertiesViewer;
