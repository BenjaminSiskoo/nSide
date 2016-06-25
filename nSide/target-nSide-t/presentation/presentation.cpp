#include "../tomoko.hpp"
unique_pointer<Presentation> presentation;

Presentation::Presentation() {
  presentation = this;

  libraryMenu.setText("Library");
  for(auto& emulator : program->emulators) {
    for(auto& medium : emulator->media) {
      if(!medium.bootable) continue;
      auto item = new MenuItem{&libraryMenu};
      item->setText({medium.name, " ..."}).onActivate([=] {
        program->loadMedium(*emulator, medium);
      });
      loadBootableMedia.append(item);
    }
  }
  //add cart-pal menu options -- but only if cart-pal binary is present
  if(execute("cart-pal", "--name").output.strip() == "cart-pal") {
    libraryMenu.append(MenuSeparator());
    libraryMenu.append(MenuItem().setText("Import ROM File ...").onActivate([&] {
      audio->clear();
      if(auto location = execute("cart-pal", "--import")) {
        //program->loadMedium(location.output.strip());
      }
    }));
    libraryMenu.append(MenuItem().setText("Import ROM Files ...").onActivate([&] {
      invoke("cart-pal");
    }));
  }

  systemMenu.setText("System").setVisible(false);
  powerSystem.setText("Power").onActivate([&] { program->powerCycle(); });
  resetSystem.setText("Reset").onActivate([&] { program->softReset(); });
  unloadSystem.setText("Unload").onActivate([&] { program->unloadMedium(); });

  settingsMenu.setText("Settings");
  videoScaleMenu.setText("Video Scale");
  if(settings["Video/Scale"].text() == "Tiny") videoScaleTiny.setChecked();
  if(settings["Video/Scale"].text() == "Small") videoScaleSmall.setChecked();
  if(settings["Video/Scale"].text() == "Medium") videoScaleMedium.setChecked();
  if(settings["Video/Scale"].text() == "Large") videoScaleLarge.setChecked();
  videoScaleTiny.setText("Tiny").onActivate([&] {
    settings["Video/Scale"].setValue("Tiny");
    resizeViewport();
  });
  videoScaleSmall.setText("Small").onActivate([&] {
    settings["Video/Scale"].setValue("Small");
    resizeViewport();
  });
  videoScaleMedium.setText("Medium").onActivate([&] {
    settings["Video/Scale"].setValue("Medium");
    resizeViewport();
  });
  videoScaleLarge.setText("Large").onActivate([&] {
    settings["Video/Scale"].setValue("Large");
    resizeViewport();
  });
  aspectCorrection.setText("Aspect Correction").setChecked(settings["Video/AspectCorrection"].boolean()).onToggle([&] {
    settings["Video/AspectCorrection"].setValue(aspectCorrection.checked());
    resizeViewport();
  });
  videoEmulationMenu.setText("Video Emulation");
  blurEmulation.setText("Blurring").setChecked(settings["Video/BlurEmulation"].boolean()).onToggle([&] {
    settings["Video/BlurEmulation"].setValue(blurEmulation.checked());
    if(emulator) emulator->set("Blur Emulation", blurEmulation.checked());
  });
  colorEmulation.setText("Colors").setChecked(settings["Video/ColorEmulation"].boolean()).onToggle([&] {
    settings["Video/ColorEmulation"].setValue(colorEmulation.checked());
    if(emulator) emulator->set("Color Emulation", colorEmulation.checked());
  });
  scanlineEmulation.setText("Scanlines").setChecked(settings["Video/ScanlineEmulation"].boolean()).setVisible(false).onToggle([&] {
    settings["Video/ScanlineEmulation"].setValue(scanlineEmulation.checked());
    if(emulator) emulator->set("Scanline Emulation", scanlineEmulation.checked());
  });
  maskOverscan.setText("Mask Overscan").setChecked(settings["Video/Overscan/Mask"].boolean()).onToggle([&] {
    settings["Video/Overscan/Mask"].setValue(maskOverscan.checked());
  });
  videoShaderMenu.setText("Video Shader");
  videoShaderNone.setText("None").onActivate([&] {
    settings["Video/Shader"].setValue("None");
    program->updateVideoShader();
  });
  videoShaderBlur.setText("Blur").onActivate([&] {
    settings["Video/Shader"].setValue("Blur");
    program->updateVideoShader();
  });
  loadShaders();
  synchronizeVideo.setText("Synchronize Video").setChecked(settings["Video/Synchronize"].boolean()).setVisible(false).onToggle([&] {
    settings["Video/Synchronize"].setValue(synchronizeVideo.checked());
    video->set(Video::Synchronize, synchronizeVideo.checked());
  });
  synchronizeAudio.setText("Synchronize Audio").setChecked(settings["Audio/Synchronize"].boolean()).onToggle([&] {
    settings["Audio/Synchronize"].setValue(synchronizeAudio.checked());
    audio->set(Audio::Synchronize, synchronizeAudio.checked());
  });
  muteAudio.setText("Mute Audio").setChecked(settings["Audio/Mute"].boolean()).onToggle([&] {
    settings["Audio/Mute"].setValue(muteAudio.checked());
    program->updateAudioEffects();
  });
  showStatusBar.setText("Show Status Bar").setChecked(settings["UserInterface/ShowStatusBar"].boolean()).onToggle([&] {
    settings["UserInterface/ShowStatusBar"].setValue(showStatusBar.checked());
    statusBar.setVisible(showStatusBar.checked());
    if(visible()) resizeViewport();
  });
  showConfiguration.setText("Configuration ...").onActivate([&] { settingsManager->show(2); });

  toolsMenu.setText("Tools").setVisible(false);
  saveStateMenu.setText("Save State");
  saveSlot0.setText("Slot 0").onActivate([&] { program->saveState(0); });
  saveSlot1.setText("Slot 1").onActivate([&] { program->saveState(1); });
  saveSlot2.setText("Slot 2").onActivate([&] { program->saveState(2); });
  saveSlot3.setText("Slot 3").onActivate([&] { program->saveState(3); });
  saveSlot4.setText("Slot 4").onActivate([&] { program->saveState(4); });
  saveSlot5.setText("Slot 5").onActivate([&] { program->saveState(5); });
  saveSlot6.setText("Slot 6").onActivate([&] { program->saveState(6); });
  saveSlot7.setText("Slot 7").onActivate([&] { program->saveState(7); });
  saveSlot8.setText("Slot 8").onActivate([&] { program->saveState(8); });
  saveSlot9.setText("Slot 9").onActivate([&] { program->saveState(9); });
  loadStateMenu.setText("Load State");
  loadSlot0.setText("Slot 0").onActivate([&] { program->loadState(0); });
  loadSlot1.setText("Slot 1").onActivate([&] { program->loadState(1); });
  loadSlot2.setText("Slot 2").onActivate([&] { program->loadState(2); });
  loadSlot3.setText("Slot 3").onActivate([&] { program->loadState(3); });
  loadSlot4.setText("Slot 4").onActivate([&] { program->loadState(4); });
  loadSlot5.setText("Slot 5").onActivate([&] { program->loadState(5); });
  loadSlot6.setText("Slot 6").onActivate([&] { program->loadState(6); });
  loadSlot7.setText("Slot 7").onActivate([&] { program->loadState(7); });
  loadSlot8.setText("Slot 8").onActivate([&] { program->loadState(8); });
  loadSlot9.setText("Slot 9").onActivate([&] { program->loadState(9); });
  cheatEditor.setText("Cheat Editor").onActivate([&] { toolsManager->show(0); });
  stateManager.setText("State Manager").onActivate([&] { toolsManager->show(1); });
  manifestViewer.setText("Manifest Viewer").onActivate([&] { toolsManager->show(2); });

  helpMenu.setText("Help");
  documentation.setText("Documentation for higan ...").onActivate([&] {
    invoke("http://doc.byuu.org/higan/");
  });
  about.setText("About ...").onActivate([&] {
    MessageDialog().setParent(*this).setTitle("About nSide ...").setText({
      Emulator::Name, " v", Emulator::Version, "\n",
      "Based on ", Emulator::OriginalName, " v", Emulator::FromVersion, "\n\n",
      "Original Author: ", Emulator::OriginalAuthor, "\n",
      "Fork Author: ", Emulator::Author, "\n",
      "Contributors: ", Emulator::Contributors, "\n",
      "License: ", Emulator::License, "\n",
      "Website of ", Emulator::OriginalName, ": ", Emulator::Website
    }).information();
  });

  statusBar.setFont(Font().setBold());
  statusBar.setVisible(settings["UserInterface/ShowStatusBar"].boolean());

  //viewport.setDroppable().onDrop([&](auto locations) { program->load(locations(0)); });

  onClose([&] { program->quit(); });

  setTitle({"nSide v", Emulator::Version});
  setResizable(false);
  setBackgroundColor({0, 0, 0});
  resizeViewport();
  setCentered();

  #if defined(PLATFORM_WINDOWS)
  Application::Windows::onModalChange([](bool modal) { if(modal && audio) audio->clear(); });
  #endif

  #if defined(PLATFORM_MACOSX)
  showConfigurationSeparator.setVisible(false);
  showConfiguration.setVisible(false);
  about.setVisible(false);
  Application::Cocoa::onAbout([&] { about.doActivate(); });
  Application::Cocoa::onActivate([&] { setFocused(); });
  Application::Cocoa::onPreferences([&] { showConfiguration.doActivate(); });
  Application::Cocoa::onQuit([&] { doClose(); });
  #endif
}

auto Presentation::updateEmulator() -> void {
  if(!emulator) return;
  resetSystem.setVisible(emulator->information.resettable);
  inputPort1.setVisible(false).reset();
  inputPort2.setVisible(false).reset();
  inputPort3.setVisible(false).reset();
  inputPort4.setVisible(false).reset();

  for(auto n : range(emulator->ports)) {
    if(n >= 4) break;
    auto& port = emulator->ports[n];
    if(!port.plugAndPlay) continue;
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
      item.setText(device.name).onActivate([=] {
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

auto Presentation::resizeViewport() -> void {
  int width   = emulator ? emulator->information.canvasWidth  : 256;
  int height  = emulator ? emulator->information.canvasHeight : 240;
  double stretch = emulator ? emulator->information.aspectRatio : 1.0;
  if(stretch != 1.0) {
    //aspect correction is always enabled in fullscreen mode
    if(!fullScreen() && !settings["Video/AspectCorrection"].boolean()) stretch = 1.0;
  }

  int scale = 2;
  if(settings["Video/Scale"].text() == "Tiny"  ) scale = 1;
  if(settings["Video/Scale"].text() == "Small" ) scale = 2;
  if(settings["Video/Scale"].text() == "Medium") scale = 3;
  if(settings["Video/Scale"].text() == "Large" ) scale = 4;

  int windowWidth = 0, windowHeight = 0;
  if(!fullScreen()) {
    windowWidth  = 256 * scale * (settings["Video/AspectCorrection"].boolean() ? stretch : 1.0);
    windowHeight = 240 * scale;
  } else {
    windowWidth  = geometry().width();
    windowHeight = geometry().height();
  }

  int multiplier = min(windowWidth / (int)(width * stretch), windowHeight / height);
  width = width * multiplier * stretch;
  height = height * multiplier;

  if(!fullScreen()) setSize({windowWidth, windowHeight});
  viewport.setGeometry({(windowWidth - width) / 2, (windowHeight - height) / 2, width, height});

  if(!emulator) drawSplashScreen();
}

auto Presentation::toggleFullScreen() -> void {
  if(fullScreen() == false) {
    menuBar.setVisible(false);
    statusBar.setVisible(false);
    setResizable(true);
    setFullScreen(true);
    if(!input->acquired()) input->acquire();
  } else {
    if(input->acquired()) input->release();
    setFullScreen(false);
    setResizable(false);
    menuBar.setVisible(true);
    statusBar.setVisible(settings["UserInterface/ShowStatusBar"].boolean());
  }

  Application::processEvents();
  resizeViewport();
}

auto Presentation::drawSplashScreen() -> void {
  if(!video) return;
  uint32_t* output;
  uint length;
  if(video->lock(output, length, 256, 240)) {
    for(auto y : range(240)) {
      auto dp = output + y * (length >> 2);
      for(auto x : range(256)) *dp++ = 0xff000000;
    }
    video->unlock();
    video->refresh();
  }
}

auto Presentation::loadShaders() -> void {
  auto pathname = locate("Video Shaders/");

  if(settings["Video/Driver"].text() == "OpenGL") {
    for(auto shader : directory::folders(pathname, "*.shader")) {
      if(videoShaders.objectCount() == 2) videoShaderMenu.append(MenuSeparator());
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

  for(auto radioItem : videoShaders.objects<MenuRadioItem>()) {
    if(settings["Video/Shader"].text() == string{pathname, radioItem.text(), ".shader/"}) {
      radioItem.setChecked();
    }
  }
}
