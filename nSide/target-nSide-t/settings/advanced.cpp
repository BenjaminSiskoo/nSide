AdvancedSettings::AdvancedSettings(TabFrame* parent) : TabFrameItem(parent) {
  setIcon(Icon::Action::Settings);
  refreshLocale();

  layout.setMargin(5);

  localeLabel.setFont(Font().setBold());
  localeSelect.onChange([&] { settings["UserInterface/Locale"].setValue(localeSelect.selected().text()); });
  ComboButtonItem localeAuto;
  localeAuto.setText("Auto");
  if(settings["UserInterface/Locale"].text() == "Auto") localeAuto.setSelected();
  localeSelect.append(localeAuto);
  for(auto& filename : directory::files(locate("Locales/"), "*.bml")) {
    if(filename == "default.bml") continue;
    string locale = string{filename}.trimRight(".bml", 1L);
    ComboButtonItem item;
    item.setText(locale);
    localeSelect.append(item);
    if(settings["UserInterface/Locale"].text() == locale) item.setSelected();
  }

  driverLabel.setFont(Font().setBold());
  videoDriver.onChange([&] { settings["Video/Driver"].setValue(videoDriver.selected().text()); });
  for(auto& driver : Video::availableDrivers()) {
    ComboButtonItem item;
    item.setText(driver);
    videoDriver.append(item);
    if(settings["Video/Driver"].text() == driver) item.setSelected();
  }
  audioDriver.onChange([&] { settings["Audio/Driver"].setValue(audioDriver.selected().text()); });
  for(auto& driver : Audio::availableDrivers()) {
    ComboButtonItem item;
    item.setText(driver);
    audioDriver.append(item);
    if(settings["Audio/Driver"].text() == driver) item.setSelected();
  }
  inputDriver.onChange([&] { settings["Input/Driver"].setValue(inputDriver.selected().text()); });
  for(auto& driver : Input::availableDrivers()) {
    ComboButtonItem item;
    item.setText(driver);
    inputDriver.append(item);
    if(settings["Input/Driver"].text() == driver) item.setSelected();
  }

  libraryLabel.setFont(Font().setBold());
  libraryLocation.setEditable(false).setText(settings["Library/Location"].text());
  libraryChange.onActivate([&] {
    if(auto location = BrowserDialog().setTitle("Select Library Location").selectFolder()) {
      settings["Library/Location"].setValue(location);
      libraryLocation.setText(location);
    }
  });

  ignoreManifests.setChecked(settings["Library/IgnoreManifests"].boolean()).onToggle([&] {
    settings["Library/IgnoreManifests"].setValue(ignoreManifests.checked());
  });

  devStateLabel.setFont(Font().setBold());

  uint devState = settings["Library/DevState"].natural();
  auto* devStateRadioLabel = &*(devStateGroup->state.objects[devState].acquire());
  dynamic_cast<mRadioLabel*>(devStateRadioLabel)->setChecked();

  devStateFull.onActivate([&] {
    settings["Library/DevState"].setValue((uint)Emulator::Interface::DevState::Full);
    presentation->refreshLibraryMenu();
    settingsManager->input.refreshEmulatorList();
  });
  devStateAlpha.onActivate([&] {
    settings["Library/DevState"].setValue((uint)Emulator::Interface::DevState::Alpha);
    presentation->refreshLibraryMenu();
    settingsManager->input.refreshEmulatorList();
  });
  devStatePreAlpha.onActivate([&] {
    settings["Library/DevState"].setValue((uint)Emulator::Interface::DevState::PreAlpha);
    presentation->refreshLibraryMenu();
    settingsManager->input.refreshEmulatorList();
  });
}

auto AdvancedSettings::refreshLocale() -> void {
  setText(locale["Settings/Advanced"]);

  localeLabel.setText("Locale:");

  driverLabel.setText(locale["Settings/Advanced/DriverSelection"]);
  videoLabel.setText(locale["Settings/Advanced/DriverSelection/Video"]);
  audioLabel.setText(locale["Settings/Advanced/DriverSelection/Audio"]);
  inputLabel.setText(locale["Settings/Advanced/DriverSelection/Input"]);

  libraryLabel.setText(locale["Settings/Advanced/GameLibrary"]);
  libraryPrefix.setText(locale["Settings/Advanced/GameLibrary/Location"]);
  libraryChange.setText(locale["Settings/Advanced/GameLibrary/Change..."]);
  ignoreManifests.setText(locale["Settings/Advanced/GameLibrary/IgnoreManifests"]);

  devStateLabel.setText(locale["Settings/Advanced/DevelopmentState"]);

  devStateFull.setText(locale["Settings/Advanced/DevelopmentState/ShowFull"]);
  devStateAlpha.setText(locale["Settings/Advanced/DevelopmentState/ShowAlpha"]);
  devStatePreAlpha.setText(locale["Settings/Advanced/DevelopmentState/ShowPreAlpha"]);
}
