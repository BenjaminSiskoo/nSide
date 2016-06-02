auto Program::loadMedium(string location) -> bool {
  unloadMedium();

  mediumPaths(0) = locateSystem("Super Famicom.sys/");
  mediumPaths(1) = location;
  folderPaths.append(location);

  directory::create({folderPaths(0), "debug/"});

  Emulator::audio.reset(2, audio->get(Audio::Frequency).get<uint>(44100));
  emulator->connect(SuperFamicom::Port::Controller1, SuperFamicom::Device::Gamepad);
  emulator->connect(SuperFamicom::Port::Controller2, SuperFamicom::Device::None);
  emulator->load(SuperFamicom::ID::SuperFamicom);
  emulator->set("Blur Emulation", false);
  emulator->set("Color Emulation", false);
  emulator->set("Scanline Emulation", false);
  emulator->power();

  presentation->setTitle(basename(folderPaths(0)).trimRight("/"));
  debugger->print(SuperFamicom::cartridge.information.markup.cartridge, "\n");
  debugger->suspend();
  return true;
}

auto Program::unloadMedium() -> void {
  emulator->unload();
  debugger->print("Cartridge unloaded\n");
  mediumPaths.reset();
  folderPaths.reset();
}
