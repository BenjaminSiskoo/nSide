auto Program::loadMedia(string location) -> bool {
  unloadMedia();

  mediaPaths(0) = locateSystem("Super Famicom.sys/");
  mediaPaths(1) = location;
  folderPaths.append(location);

  directory::create({folderPaths(0), "debug/"});

  Emulator::audio.reset();
  Emulator::audio.setFrequency(audio->get(Audio::Frequency).get<uint>());
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

auto Program::unloadMedia() -> void {
  emulator->unload();
  debugger->print("Cartridge unloaded\n");
  mediaPaths.reset();
  folderPaths.reset();
}
