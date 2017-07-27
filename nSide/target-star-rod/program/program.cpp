#include "../laevateinn.hpp"
#include "interface.cpp"
#include "medium.cpp"
#include "state.cpp"
#include "utility.cpp"
unique_pointer<Program> program;

Program::Program(string_vector args) {
  program = this;

  Emulator::platform = this;
  emulator = new SFC::Interface;

  new Settings;
  new Debugger;
  new Tracer;
  new ConsoleWindow;
  new AboutWindow;
  new Presentation;
  new CPUDebugger;
  new CPURegisterEditor;
  new SMPDebugger;
  new SMPRegisterEditor;
  new MemoryEditor;
  new BreakpointEditor;
  new PropertiesViewer;
  new VRAMViewer;
  new BGViewer;
  new PaletteViewer;

  higan_settings = BML::unserialize(string::read(locateHigan("settings.bml")));

  video = Video::create();
  video->setContext(presentation->viewport.handle());
  video->setBlocking(settings["Video/Synchronize"].boolean());
  video->setSmooth(false);
  if(!video->ready()) debugger->print("Failed to initialize video driver\n");

  audio = Audio::create();
  audio->setExclusive(false);
  audio->setContext(presentation->viewport.handle());
  audio->setDevice("");
  audio->setBlocking(settings["Audio/Synchronize"].boolean());
  audio->setChannels(2);
  if(!audio->ready()) debugger->print("Failed to initialize audio driver\n");

  input = Input::create();
  input->setContext(presentation->viewport.handle());
  if(!input->ready()) debugger->print("Failed to initialize input driver\n");

  args.takeLeft();  //ignore program location in argument parsing
  for(auto& argument : args) {
    if(directory::exists(argument.split("|", 1L).right())) {
      if(!argument.transform("\\", "/").endsWith("/")) argument.append("/");
      mediumQueue.append(argument);
    }
  }
  loadMedium(*emulator, emulator->media[0]);

  consoleWindow->setVisible();
  presentation->setVisible();
  consoleWindow->setFocused();
  presentation->drawSplashScreen();

  cpuDebugger->updateDisassembly();
  smpDebugger->updateDisassembly();
  memoryEditor->selectSource();
  propertiesViewer->updateProperties();
  vramViewer->updateTiles();
  bgViewer->updateTiles();
  paletteViewer->updateColors();

  Application::onMain({&Program::main, this});
}

auto Program::main() -> void {
  debugger->run();
}

auto Program::quit() -> void {
  unloadMedium();
  settings.quit();
  Application::quit();
}
