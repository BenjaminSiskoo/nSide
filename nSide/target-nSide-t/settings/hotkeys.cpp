HotkeySettings::HotkeySettings(TabFrame* parent) : TabFrameItem(parent) {
  setIcon(Icon::Device::Keyboard);
  refreshLocale();

  layout.setMargin(5);
  mappingList.onActivate([&] { assignMapping(); });
  mappingList.onChange([&] {
    eraseButton.setEnabled((bool)mappingList.selected());
  });
  toggleLogicButton.onActivate([&] {
    if(auto item = mappingList.selected()) {
      inputManager->hotkeys[item.offset()]->toggleLogic();
      refreshMappings();
    }
  });
  resetButton.onActivate([&] {
    if(MessageDialog("Are you sure you want to erase all hotkey mappings?").setParent(*settingsManager).question() == "Yes") {
      for(auto& mapping : inputManager->hotkeys) mapping->unbind();
      refreshMappings();
    }
  });
  eraseButton.onActivate([&] {
    if(auto item = mappingList.selected()) {
      inputManager->hotkeys[item.offset()]->unbind();
      refreshMappings();
    }
  });

  reloadMappings();
  refreshMappings();
}

auto HotkeySettings::refreshLocale() -> void {
  setText(locale["Settings/Hotkeys"]);

  toggleLogicButton.setText(locale["Settings/Hotkeys/ToggleLogic"]);
  resetButton.setText(locale["Settings/Hotkeys/Reset"]);
  eraseButton.setText(locale["Settings/Hotkeys/Erase"]);
}

auto HotkeySettings::reloadMappings() -> void {
  mappingList.reset();
  mappingList.append(TableViewHeader().setVisible()
    .append(TableViewColumn().setText(locale["Settings/Hotkeys/Name"]))
    .append(TableViewColumn().setText(locale["Settings/Hotkeys/Mapping"]).setExpandable())
  );
  for(auto& hotkey : inputManager->hotkeys) {
    mappingList.append(TableViewItem()
      .append(TableViewCell().setText(hotkey->name))
      .append(TableViewCell())
    );
  }
  mappingList.resizeColumns();
}

auto HotkeySettings::refreshMappings() -> void {
  uint position = 0;
  for(auto& hotkey : inputManager->hotkeys) {
    mappingList.item(position).cell(1).setText(hotkey->displayName());
    position++;
  }
  mappingList.resizeColumns();
}

auto HotkeySettings::assignMapping() -> void {
  inputManager->poll();  //clear any pending events first

  if(auto item = mappingList.selected()) {
    activeMapping = inputManager->hotkeys[item.offset()];
    settingsManager->layout.setEnabled(false);
    settingsManager->statusBar.setText(locale["Settings/Hotkeys/PressKey"].replace("%s", activeMapping->name));
  }
}

auto HotkeySettings::inputEvent(shared_pointer<HID::Device> device, uint group, uint input, int16 oldValue, int16 newValue) -> void {
  if(!activeMapping) return;
  if(device->isMouse()) return;

  if(activeMapping->bind(device, group, input, oldValue, newValue)) {
    activeMapping = nullptr;
    settingsManager->statusBar.setText(locale["Settings/Hotkeys/MappingAssigned"]);
    refreshMappings();
    timer.onActivate([&] {
      timer.setEnabled(false);
      settingsManager->statusBar.setText();
      settingsManager->layout.setEnabled();
    }).setInterval(200).setEnabled();
  }
}
