struct PropertiesViewer : Window {
  VerticalLayout layout;
    HorizontalLayout controlLayout;
      Label sourceLabel;
      ComboButton sourceSelection;
      Widget spacer;
      CheckLabel autoUpdate;
      Button update;
    TextEdit properties;

  void updateProperties();
  PropertiesViewer();
};

extern PropertiesViewer* propertiesViewer;
