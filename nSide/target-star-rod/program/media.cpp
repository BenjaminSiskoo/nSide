auto Program::loadMedia(string location) -> bool {
  unloadMedia();

  mediaPaths(0) = locateSystem("Super Famicom.sys/");
  mediaPaths(1) = location;
  folderPaths.append(location);

  directory::create({folderPaths(0), "debug/"});

  emulator->load(SuperFamicom::ID::SuperFamicom);
  emulator->power();
  emulator->connect((uint)SuperFamicom::Device::Port::Controller1, (uint)SuperFamicom::Device::ID::Gamepad);
  emulator->set("Blur Emulation", false);
  emulator->set("Color Emulation", false);
  emulator->set("Scanline Emulation", false);

  presentation->setTitle(basename(folderPaths(0)).rtrim("/"));
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
