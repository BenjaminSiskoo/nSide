auto Program::loadMedium(Emulator::Interface& interface, const Emulator::Interface::Medium& medium) -> void {
  unloadMedium();

  mediumPaths.append(locateSystem({medium.name, ".sys/"}));

  Emulator::audio.reset(2, audio->get(Audio::Frequency).get<uint>(44100));
  emulator = &interface;
  connectDevices();
  if(!emulator->load(medium.id)) {
    emulator = nullptr;
    mediumPaths.reset();
    return;
  }
  updateAudioDriver();
  updateAudioEffects();
  emulator->power();

  presentation->resizeViewport();
  presentation->setTitle(emulator->title());
  presentation->systemMenu.setText(medium.name).setVisible(true);
  presentation->toolsMenu.setVisible(true);
  presentation->updateEmulator();
  toolsManager->cheatEditor.loadCheats();
  toolsManager->stateManager.doRefresh();
  toolsManager->manifestViewer.doRefresh();
}

auto Program::unloadMedium() -> void {
  if(!emulator) return;

  toolsManager->cheatEditor.saveCheats();
  emulator->unload();
  emulator = nullptr;
  mediumPaths.reset();

  presentation->drawSplashScreen();
  presentation->setTitle({"nSide v", Emulator::Version});
  presentation->systemMenu.setVisible(false);
  presentation->toolsMenu.setVisible(false);
  toolsManager->setVisible(false);
}
