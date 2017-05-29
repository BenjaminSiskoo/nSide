auto InputManager::appendHotkeys() -> void {
  { auto hotkey = new InputHotkey;
    hotkey->name = "Toggle Fullscreen";
    hotkey->press = [] {
      presentation->toggleFullScreen();
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Toggle Mouse Capture";
    hotkey->press = [] {
      input->acquired() ? input->release() : input->acquire();
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Save State";
    hotkey->press = [] {
      program->saveState(0);
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Load State";
    hotkey->press = [] {
      program->loadState(0);
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Pause Emulation";
    hotkey->press = [] {
      program->pause = !program->pause;
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Fast Forward";
    hotkey->press = [] {
      video->set(Video::Synchronize, false);
      audio->set(Audio::Synchronize, false);
    };
    hotkey->release = [] {
      video->set(Video::Synchronize, settings["Video/Synchronize"].boolean());
      audio->set(Audio::Synchronize, settings["Audio/Synchronize"].boolean());
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Power Cycle";
    hotkey->press = [] {
      program->powerCycle();
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Rotate";
    hotkey->press = [] {
      program->rotate();
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Save Screenshot";
    hotkey->press = [] {
      if(!::emulator) return;
      uint counter = directory::files(program->mediumPaths.right(), "screenshot????.png").size();
      Emulator::video.screenshot({program->mediumPaths.right(), "screenshot", pad(counter, 4, '0'), ".png"});
      program->showMessage("Screenshot saved");
    };
    hotkeys.append(hotkey);
  }

  { auto hotkey = new InputHotkey;
    hotkey->name = "Export Memory";
    hotkey->press = [] {
      if(!::emulator) return;
      ::emulator->exportMemory();
      program->showMessage("Memory exported");
    };
    hotkeys.append(hotkey);
  }

  for(auto& hotkey : hotkeys) {
    hotkey->path = string{"Hotkey/", hotkey->name}.replace(" ", "");
    hotkey->assignment = settings(hotkey->path).text();
    hotkey->bind();
  }
}

auto InputManager::pollHotkeys() -> void {
  if(!presentation || !presentation->focused()) return;

  for(auto& hotkey : hotkeys) {
    int16 state = hotkey->poll();
    if(hotkey->state == 0 && state == 1 && hotkey->press) hotkey->press();
    if(hotkey->state == 1 && state == 0 && hotkey->release) hotkey->release();
    hotkey->state = state;
  }

  static shared_pointer<HID::Device> keyboard = nullptr;
  if(!keyboard) {
    for(auto& device : inputManager->devices) {
      if(device->isKeyboard()) keyboard = device;
    }
  }

  //Unload/Quit hotkey
  //Because stock higan and nSide don't have this hotkey and cede to the OS's
  //global exit hotkey (Alt+F4 on Windows, etc.), which doesn't work in
  //fullscreen. Key code 0 represents Escape in all of ruby's input drivers.
  static bool previousEsc = 0;
  auto esc = keyboard->group(0).input(0).value();
  if(previousEsc == 0 && esc == 1) {
    if(::emulator) program->unloadMedium();
    else           program->quit();
  }
  previousEsc = esc;

  if(settings["Input/Driver"].text() == "Windows") {
    static shared_pointer<HID::Device> xinput1 = nullptr;
    if(!xinput1) {
      for(auto& device : inputManager->devices) {
        if(device->isJoypad() && device->id() == 0x1'045e'028e) xinput1 = device;
      }
    }

    if(xinput1) {
      auto guide = xinput1->group(HID::Joypad::GroupID::Button).input(10).value();
      if(guide == 1 && ::emulator) program->unloadMedium();
    }
  }
}
