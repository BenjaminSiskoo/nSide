auto Program::loadMedium() -> void {
  if(!mediumQueue) return;

  string location = mediumQueue.left();
  string type = suffixname(location).trimLeft(".", 1L);

  for(auto& medium : emulator->media) {
    if(medium.type != type) continue;
    return loadMedium(*emulator, medium);
  }

  mediumQueue.reset();
}

auto Program::loadMedium(Emulator::Interface& interface, const Emulator::Interface::Medium& medium) -> void {
  unloadMedium();

  mediumPaths.append(locateSystem({medium.name, ".sys/"}));
  debugger->print(medium.name, "\n");

  Emulator::audio.reset(2, audio->get(Audio::Frequency).get<uint>(44100));
  emulator->connect(SuperFamicom::ID::Port::Controller1, SuperFamicom::ID::Device::Gamepad);
  emulator->connect(SuperFamicom::ID::Port::Controller2, SuperFamicom::ID::Device::None);
  emulator->load(medium.id);
  emulator->set("Blur Emulation", false);
  emulator->set("Color Emulation", false);
  emulator->set("Scanline Emulation", false);
  emulator->power();

  presentation->setTitle(basename(mediumPaths(1)).trimRight("/"));
  debugger->print(SuperFamicom::cartridge.information.manifest.cartridge, "\n");
  debugger->suspend();
}

auto Program::unloadMedium() -> void {
  emulator->unload();
  debugger->print("Cartridge unloaded\n");
  mediumPaths.reset();
}
