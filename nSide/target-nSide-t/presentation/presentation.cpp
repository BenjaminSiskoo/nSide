#include "../nSide.hpp"
#include "about.cpp"
unique_pointer<AboutWindow> aboutWindow;
unique_pointer<Presentation> presentation;

Presentation::Presentation() {
  presentation = this;

  refreshLocale();

  systemMenu.setVisible(false);
  reloadSystem.onActivate([&] { program->powerCycle(); });
  unloadSystem.onActivate([&] { program->unloadMedium(); });

  videoScaleSmall.onActivate([&] {
    settings["Video/Windowed/Scale"].setValue("Small");
    resizeViewport();
  });
  videoScaleMedium.onActivate([&] {
    settings["Video/Windowed/Scale"].setValue("Medium");
    resizeViewport();
  });
  videoScaleLarge.onActivate([&] {
    settings["Video/Windowed/Scale"].setValue("Large");
    resizeViewport();
  });
  blurEmulation.setChecked(settings["Video/BlurEmulation"].boolean()).onToggle([&] {
    settings["Video/BlurEmulation"].setValue(blurEmulation.checked());
    if(emulator) emulator->set("Blur Emulation", blurEmulation.checked());
  });
  colorEmulation.setChecked(settings["Video/ColorEmulation"].boolean()).onToggle([&] {
    settings["Video/ColorEmulation"].setValue(colorEmulation.checked());
    if(emulator) emulator->set("Color Emulation", colorEmulation.checked());
  });
  scanlineEmulation.setChecked(settings["Video/ScanlineEmulation"].boolean()).setVisible(false).onToggle([&] {
    settings["Video/ScanlineEmulation"].setValue(scanlineEmulation.checked());
    if(emulator) emulator->set("Scanline Emulation", scanlineEmulation.checked());
  });
  videoShaderNone.onActivate([&] {
    settings["Video/Shader"].setValue("None");
    program->updateVideoShader();
  });
  videoShaderBlur.onActivate([&] {
    settings["Video/Shader"].setValue("Blur");
    program->updateVideoShader();
  });
//videoShaderAuto.onActivate([&] {
//  settings["Video/Shader"].setValue("Auto");
//  program->updateVideoShader();
//});
  loadShaders();
  synchronizeVideo.setChecked(settings["Video/Synchronize"].boolean()).setVisible(false).onToggle([&] {
    settings["Video/Synchronize"].setValue(synchronizeVideo.checked());
    video->setBlocking(synchronizeVideo.checked());
  });
  synchronizeAudio.setChecked(settings["Audio/Synchronize"].boolean()).onToggle([&] {
    settings["Audio/Synchronize"].setValue(synchronizeAudio.checked());
    audio->setBlocking(synchronizeAudio.checked());
  });
  muteAudio.setChecked(settings["Audio/Mute"].boolean()).onToggle([&] {
    settings["Audio/Mute"].setValue(muteAudio.checked());
    program->updateAudioEffects();
  });
  showStatusBar.setChecked(settings["UserInterface/ShowStatusBar"].boolean()).onToggle([&] {
    settings["UserInterface/ShowStatusBar"].setValue(showStatusBar.checked());
    statusBar.setVisible(showStatusBar.checked());
    if(visible()) resizeViewport();
  });
  showVideoSettings.onActivate([&] { settingsManager->show(0); });
  showAudioSettings.onActivate([&] { settingsManager->show(1); });
  showInputSettings.onActivate([&] {
    if(emulator) {
      //default input panel to current core's input settings
      for(auto item : settingsManager->input.emulatorList.items()) {
        if(systemMenu.text() == item.text()) {
          item.setSelected();
          settingsManager->input.emulatorList.doChange();
          break;
        }
      }
    }
    settingsManager->show(2);
  });
  showHotkeySettings.onActivate([&] { settingsManager->show(3); });
  showAdvancedSettings.onActivate([&] { settingsManager->show(4); });

  toolsMenu.setVisible(false);
  saveSlot0.onActivate([&] { program->saveState(0); });
  saveSlot1.onActivate([&] { program->saveState(1); });
  saveSlot2.onActivate([&] { program->saveState(2); });
  saveSlot3.onActivate([&] { program->saveState(3); });
  saveSlot4.onActivate([&] { program->saveState(4); });
  saveSlot5.onActivate([&] { program->saveState(5); });
  saveSlot6.onActivate([&] { program->saveState(6); });
  saveSlot7.onActivate([&] { program->saveState(7); });
  saveSlot8.onActivate([&] { program->saveState(8); });
  saveSlot9.onActivate([&] { program->saveState(9); });
  loadSlot0.onActivate([&] { program->loadState(0); });
  loadSlot1.onActivate([&] { program->loadState(1); });
  loadSlot2.onActivate([&] { program->loadState(2); });
  loadSlot3.onActivate([&] { program->loadState(3); });
  loadSlot4.onActivate([&] { program->loadState(4); });
  loadSlot5.onActivate([&] { program->loadState(5); });
  loadSlot6.onActivate([&] { program->loadState(6); });
  loadSlot7.onActivate([&] { program->loadState(7); });
  loadSlot8.onActivate([&] { program->loadState(8); });
  loadSlot9.onActivate([&] { program->loadState(9); });
  cheatEditor.onActivate([&] { toolsManager->show(0); });
  stateManager.onActivate([&] { toolsManager->show(1); });
  manifestViewer.onActivate([&] { toolsManager->show(2); });

  documentation.onActivate([&] {
    invoke("https://doc.byuu.org/higan/");
  });
  //Wait for this web page to go online before adding this menu item
//credits.setText("Credits ...").onActivate([&] {
//  invoke("https://doc.byuu.org/higan/credits/");
//});
  about.onActivate([&] {
    aboutWindow->setVisible().setFocused();
  });

  statusBar.setFont(Font().setBold());
  statusBar.setVisible(settings["UserInterface/ShowStatusBar"].boolean());

  viewport.setDroppable().onDrop([&](auto locations) {
    if(!directory::exists(locations(0))) return;
    program->mediumQueue.append(locations(0));
    program->loadMedium();
  });

  onSize([&] {
    resizeViewport(false);
  });

  onClose([&] {
    program->quit();
  });

  setTitle({"nSide v", Emulator::Version});
  setBackgroundColor({0, 0, 0});
  resizeViewport();
  setCentered();

  #if defined(PLATFORM_WINDOWS)
  Application::Windows::onModalChange([](bool modal) { if(modal && audio) audio->clear(); });
  #endif

  #if defined(PLATFORM_MACOSX)
  about.setVisible(false);
  Application::Cocoa::onAbout([&] { about.doActivate(); });
  Application::Cocoa::onActivate([&] { setFocused(); });
  Application::Cocoa::onPreferences([&] { showInputSettings.doActivate(); });
  Application::Cocoa::onQuit([&] { doClose(); });
  #endif
}

auto Presentation::refreshLocale() -> void {
  libraryMenu.setText(locale["Menu/Library"]);
  refreshLibraryMenu();

  systemMenu.setText(locale["Menu/System"]);
  reloadSystem.setText(locale["Menu/System/PowerCycle"]);
  unloadSystem.setText(locale["Menu/System/Unload"]);

  settingsMenu.setText(locale["Menu/Settings"]);
  videoScaleMenu.setText(locale["Menu/Settings/VideoScale"]);
  videoScaleSmall.setText(locale["Menu/Settings/VideoScale/Small"]);
  videoScaleMedium.setText(locale["Menu/Settings/VideoScale/Medium"]);
  videoScaleLarge.setText(locale["Menu/Settings/VideoScale/Large"]);
  videoEmulationMenu.setText(locale["Menu/Settings/VideoEmulation"]);
  blurEmulation.setText(locale["Menu/Settings/VideoEmulation/Blurring"]);
  colorEmulation.setText(locale["Menu/Settings/VideoEmulation/Colors"]);
  scanlineEmulation.setText(locale["Menu/Settings/VideoEmulation/Scanlines"]);
  videoShaderMenu.setText(locale["Menu/Settings/VideoShader"]);
  videoShaderNone.setText(locale["Menu/Settings/VideoShader/None"]);
  videoShaderBlur.setText(locale["Menu/Settings/VideoShader/Blur"]);
//videoShaderAuto.setText(locale["Menu/Settings/VideoShader/Auto"]);
  synchronizeVideo.setText(locale["Menu/Settings/SynchronizeVideo"]);
  synchronizeAudio.setText(locale["Menu/Settings/SynchronizeAudio"]);
  muteAudio.setText(locale["Menu/Settings/MuteAudio"]);
  showStatusBar.setText(locale["Menu/Settings/ShowStatusBar"]);
  showVideoSettings.setText(locale["Menu/Settings/Video..."]);
  showAudioSettings.setText(locale["Menu/Settings/Audio..."]);
  showInputSettings.setText(locale["Menu/Settings/Input..."]);
  showHotkeySettings.setText(locale["Menu/Settings/Hotkeys..."]);
  showAdvancedSettings.setText(locale["Menu/Settings/Advanced..."]);

  toolsMenu.setText(locale["Menu/Tools"]).setVisible(false);
  saveQuickStateMenu.setText(locale["Menu/Tools/SaveQuickState"]);
  saveSlot0.setText(locale["Menu/Tools/Slot"].replace("%d", "0"));
  saveSlot1.setText(locale["Menu/Tools/Slot"].replace("%d", "1"));
  saveSlot2.setText(locale["Menu/Tools/Slot"].replace("%d", "2"));
  saveSlot3.setText(locale["Menu/Tools/Slot"].replace("%d", "3"));
  saveSlot4.setText(locale["Menu/Tools/Slot"].replace("%d", "4"));
  saveSlot5.setText(locale["Menu/Tools/Slot"].replace("%d", "5"));
  saveSlot6.setText(locale["Menu/Tools/Slot"].replace("%d", "6"));
  saveSlot7.setText(locale["Menu/Tools/Slot"].replace("%d", "7"));
  saveSlot8.setText(locale["Menu/Tools/Slot"].replace("%d", "8"));
  saveSlot9.setText(locale["Menu/Tools/Slot"].replace("%d", "9"));
  loadQuickStateMenu.setText(locale["Menu/Tools/LoadQuickState"]);
  loadSlot0.setText(locale["Menu/Tools/Slot"].replace("%d", "0"));
  loadSlot1.setText(locale["Menu/Tools/Slot"].replace("%d", "1"));
  loadSlot2.setText(locale["Menu/Tools/Slot"].replace("%d", "2"));
  loadSlot3.setText(locale["Menu/Tools/Slot"].replace("%d", "3"));
  loadSlot4.setText(locale["Menu/Tools/Slot"].replace("%d", "4"));
  loadSlot5.setText(locale["Menu/Tools/Slot"].replace("%d", "5"));
  loadSlot6.setText(locale["Menu/Tools/Slot"].replace("%d", "6"));
  loadSlot7.setText(locale["Menu/Tools/Slot"].replace("%d", "7"));
  loadSlot8.setText(locale["Menu/Tools/Slot"].replace("%d", "8"));
  loadSlot9.setText(locale["Menu/Tools/Slot"].replace("%d", "9"));
  cheatEditor.setText(locale["Menu/Tools/CheatEditor..."]);
  stateManager.setText(locale["Menu/Tools/StateManager..."]);
  manifestViewer.setText(locale["Menu/Tools/ManifestViewer..."]);

  helpMenu.setText(locale["Menu/Help"]);
  documentation.setText(locale["Menu/Help/Documentation..."]);
  about.setText(locale["Menu/Help/About..."]);
}

auto Presentation::refreshLibraryMenu() -> void {
  libraryMenu.reset();
  string_vector manufacturers;
  for(auto& emulator : program->emulators) {
    if(emulator->information.devState > settings["Library/DevState"].natural()) continue;
    if(!manufacturers.find(emulator->information.manufacturer)) {
      manufacturers.append(emulator->information.manufacturer);
    }
  }
  for(auto& manufacturer : manufacturers) {
    Menu manufacturerMenu{&libraryMenu};
    manufacturerMenu.setText(locale[{"System/", string{manufacturer}.replace(" ", "")}]);
    for(auto& emulator : program->emulators) {
      if(emulator->information.manufacturer != manufacturer) continue;
      if(emulator->information.devState > settings["Library/DevState"].natural()) continue;
      for(auto& medium : emulator->media) {
        auto item = new MenuItem{&manufacturerMenu};
        string prefix = emulator->information.devState == 2 ? "(!) " : "";
        string name = locale[{"System/", manufacturer, "/", string{medium.name}.replace(" ", "")}];
        item->setText({prefix, name, " ..."}).onActivate([=] {
          program->loadMedium(*emulator, medium);
        });
      }
    }
  }
  //add cart-pal menu options -- but only if cart-pal binary is present
  if(execute("cart-pal", "--name").output.strip() == "cart-pal") {
    libraryMenu.append(MenuSeparator());
    libraryMenu.append(MenuItem().setText(locale["Menu/Library/LoadROMFile..."]).onActivate([&] {
      audio->clear();
      if(auto location = execute("cart-pal", "--import")) {
        program->mediumQueue.append(location.output.strip());
        program->loadMedium();
      }
    }));
    libraryMenu.append(MenuItem().setText(locale["Menu/Library/ImportROMFiles..."]).onActivate([&] {
      invoke("cart-pal");
    }));
  }
}

auto Presentation::updateEmulator() -> void {
  if(!emulator) return;
  inputPort1.setVisible(false).reset();
  inputPort2.setVisible(false).reset();
  inputPort3.setVisible(false).reset();
  inputPort4.setVisible(false).reset();

  for(auto n : range(emulator->ports)) {
    if(n >= 4) break;
    auto& port = emulator->ports[n];
    auto& menu = (
    n == 0 ? inputPort1 :
    n == 1 ? inputPort2 :
    n == 2 ? inputPort3 :
             inputPort4
    );
    menu.setText(port.name);

    Group devices;
    for(auto& device : port.devices) {
      MenuRadioItem item{&menu};
      item.setText(device.name).setProperty("deviceID", device.id).onActivate([=] {
        auto path = string{emulator->information.name, "/", port.name}.replace(" ", "");
        settings[path].setValue(device.name);
        emulator->connect(port.id, device.id);
      });
      devices.append(item);
    }
    if(devices.objectCount() > 1) {
      auto path = string{emulator->information.name, "/", port.name}.replace(" ", "");
      auto device = settings(path).text();
      for(auto item : devices.objects<MenuRadioItem>()) {
        if(item.text() == device) item.setChecked();
      }
      menu.setVisible();
    }
  }

  systemMenuSeparatorPorts.setVisible(
    inputPort1.visible() ||
    inputPort2.visible() ||
    inputPort3.visible() ||
    inputPort4.visible()
  );

  emulator->set("Blur Emulation", blurEmulation.checked());
  emulator->set("Color Emulation", colorEmulation.checked());
  emulator->set("Scanline Emulation", scanlineEmulation.checked());
}

auto Presentation::clearViewport() -> void {
  if(!video) return;

  uint32_t* output;
  uint length = 0;
  uint width = viewport.geometry().width();
  uint height = viewport.geometry().height();
  if(video->lock(output, length, width, height)) {
    for(uint y : range(height)) {
      auto dp = output + y * (length >> 2);
      for(uint x : range(width)) *dp++ = 0xff000000;
    }

    video->unlock();
    video->output();
  }
}

auto Presentation::resizeViewport(bool resizeWindow) -> void {
  //clear video area before resizing to avoid seeing distorted video momentarily
  clearViewport();

  uint viewportWidth = geometry().width();
  uint viewportHeight = geometry().height();

  double emulatorWidth = 320;
  double emulatorHeight = 240;
  double aspectCorrection = 1.0;
  if(emulator) {
    auto resolution = emulator->videoResolution();
    emulatorWidth = resolution.width;
    emulatorHeight = resolution.height;
    aspectCorrection = resolution.aspectCorrection;
    if(emulator->information.overscan) {
      uint overscanHorizontal = settings["Video/Overscan/Horizontal"].natural();
      uint overscanVertical = settings["Video/Overscan/Vertical"].natural();
      emulatorWidth -= overscanHorizontal * 2;
      emulatorHeight -= overscanVertical * 2;
    }
  }

  if(!fullScreen()) {
    if(settings["Video/Windowed/AspectCorrection"].boolean()) emulatorWidth *= aspectCorrection;

    if(resizeWindow) {
      string viewportScale = "640x480";
      if(settings["Video/Windowed/Scale"].text() == "Small") viewportScale = settings["Video/Windowed/Scale/Small"].text();
      if(settings["Video/Windowed/Scale"].text() == "Medium") viewportScale = settings["Video/Windowed/Scale/Medium"].text();
      if(settings["Video/Windowed/Scale"].text() == "Large") viewportScale = settings["Video/Windowed/Scale/Large"].text();
      auto resolution = viewportScale.isplit("x", 1L);
      viewportWidth = resolution(0).natural();
      viewportHeight = resolution(1).natural();
    }

    if(settings["Video/Windowed/Adaptive"].boolean() && resizeWindow) {
      uint multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
      setSize({viewportWidth = emulatorWidth, viewportHeight = emulatorHeight});
    } else if(settings["Video/Windowed/IntegralScaling"].boolean()) {
      uint multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
      if(resizeWindow) setSize({viewportWidth, viewportHeight});
    } else {
      double multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
      if(resizeWindow) setSize({viewportWidth, viewportHeight});
    }
  } else {
    if(settings["Video/Fullscreen/AspectCorrection"].boolean()) emulatorWidth *= aspectCorrection;

    if(settings["Video/Fullscreen/IntegralScaling"].boolean()) {
      uint multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
    } else {
      double multiplier = min(viewportWidth / emulatorWidth, viewportHeight / emulatorHeight);
      emulatorWidth *= multiplier;
      emulatorHeight *= multiplier;
    }
  }

  viewport.setGeometry({
    (viewportWidth - emulatorWidth) / 2, (viewportHeight - emulatorHeight) / 2,
    emulatorWidth, emulatorHeight
  });

  //clear video area again to ensure entire viewport area has been painted in
  clearViewport();
}

auto Presentation::toggleFullScreen() -> void {
  if(!fullScreen()) {
    statusBar.setVisible(false);
    menuBar.setVisible(false);
    setFullScreen(true);
    video->setExclusive(settings["Video/Fullscreen/Exclusive"].boolean());
    if(video->exclusive()) setVisible(false);
    if(!input->acquired()) input->acquire();
  } else {
    if(input->acquired()) input->release();
    if(video->exclusive()) setVisible(true);
    video->setExclusive(false);
    setFullScreen(false);
    menuBar.setVisible(true);
    statusBar.setVisible(settings["UserInterface/ShowStatusBar"].boolean());
  }
  resizeViewport();
}

/*
auto Presentation::draw(image logo) -> void {
  if(!video) return;

  uint32_t* output;
  uint length = 0;
  uint width = viewport.geometry().width();
  uint height = viewport.geometry().height();
  if(video->lock(output, length, width, height)) {
    uint cx = (width - logo.width()) - 10;
    uint cy = (height - logo.height()) - 10;

    image backdrop;
    backdrop.allocate(width, height);
    if(logo && !program->hasQuit) {
    //backdrop.sphericalGradient(0xff0000bf, 0xff000000, logo.width(), logo.height() / 2, width, height);
      backdrop.impose(image::blend::sourceAlpha, cx, cy, logo, 0, 0, logo.width(), logo.height());
    } else {
      backdrop.fill(0xff000000);
    }

    auto data = (uint32_t*)backdrop.data();
    for(auto y : range(height)) {
      auto dp = output + y * (length >> 2);
      auto sp = data + y * width;
      for(auto x : range(width)) *dp++ = *sp++;
    }

    video->unlock();
    video->refresh();
  }
}
*/

auto Presentation::loadShaders() -> void {
  auto pathname = locate("Video Shaders/");

  if(settings["Video/Driver"].text() == "OpenGL") {
    for(auto shader : directory::folders(pathname, "*.shader")) {
      if(videoShaders.objectCount() == 2/*3*/) videoShaderMenu.append(MenuSeparator());
      MenuRadioItem item{&videoShaderMenu};
      item.setText(string{shader}.trimRight(".shader/", 1L)).onActivate([=] {
        settings["Video/Shader"].setValue({pathname, shader});
        program->updateVideoShader();
      });
      videoShaders.append(item);
    }
  }

  if(settings["Video/Shader"].text() == "None") videoShaderNone.setChecked();
  if(settings["Video/Shader"].text() == "Blur") videoShaderBlur.setChecked();
//if(settings["Video/Shader"].text() == "Auto") videoShaderAuto.setChecked();

  for(auto radioItem : videoShaders.objects<MenuRadioItem>()) {
    if(settings["Video/Shader"].text() == string{pathname, radioItem.text(), ".shader/"}) {
      radioItem.setChecked();
    }
  }
}
