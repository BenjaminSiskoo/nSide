#include "../laevateinn.hpp"
#include "interface.cpp"
#include "medium.cpp"
#include "state.cpp"
#include "utility.cpp"
unique_pointer<Program> program;

Program::Program(string_vector args) {
  program = this;
  Application::onMain({&Program::main, this});

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
  video->set(Video::Handle, presentation->viewport.handle());
  if(!video->init()) video = Video::create("None");
  video->set(Video::Synchronize, settings["Video/Synchronize"].boolean());
  video->set(Video::Filter, Video::FilterNearest);

  audio = Audio::create();
  audio->set(Audio::Handle, presentation->viewport.handle());
  audio->set(Audio::Synchronize, settings["Audio/Synchronize"].boolean());
  audio->set(Audio::Exclusive, false);
  audio->set(Audio::Latency, 80u);
  if(!audio->init()) audio = Audio::create("None");

  input = Input::create();
  input->set(Input::Handle, presentation->viewport.handle());
  if(!input->init()) input = Input::create("None");

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
}

auto Program::main() -> void {
  debugger->run();
}

auto Program::quit() -> void {
  unloadMedium();
  settings.quit();
  Application::quit();
}
