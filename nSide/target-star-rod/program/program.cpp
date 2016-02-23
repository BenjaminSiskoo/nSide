#include "../laevateinn.hpp"
#include "interface.cpp"
#include "media.cpp"
#include "state.cpp"
#include "utility.cpp"
unique_pointer<Program> program;

Program::Program(lstring args) {
  program = this;
  Application::onMain({&Program::main, this});

  emulator = new SuperFamicom::Interface;
  emulator->bind = this;

  new Settings;
  new Debugger;
  new Tracer;
  new WindowManager;
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

  higan_settings = BML::unserialize(string::read(locateHigan("settings.bml")));

  settings->load();

  string path = higan_settings["Library/Location"].text().transform("\\", "/");
  if(path.empty()) path = {nall::userpath(), "Emulation/"};
  if(path.endsWith("/") == false) path.append("/");
  path.append("Super Famicom/");

  string location;
  args.takeFirst();  //ignore program location in argument parsing
  for(auto& argument : args) {
    location = argument.transform("\\", "/").rtrim("/").append("/");
  }
  if(!directory::exists(location)) location = BrowserDialog()
  .setTitle("Load Super Famicom")
  .setPath({higan_settings["Library/Location"].text(), "Super Famicom"})
  .setFilters("Super Famicom|*.sfc")
  .openFolder();
  if(!location.endsWith(".sfc/")) return;
  if(!directory::exists(location)) return;

  windowManager->loadGeometry();
  consoleWindow->setVisible();
  presentation->setVisible();
  consoleWindow->setFocused();

  video = Video::create(settings->video.driver);
  video->set(Video::Handle, presentation->viewport.handle());
  if(!video->init()) video = Video::create("None");
  video->set(Video::Synchronize, settings->video.synchronize);
  video->set(Video::Filter, Video::FilterNearest);

  audio = Audio::create(settings->audio.driver);
  audio->set(Audio::Handle, presentation->viewport.handle());
  audio->set(Audio::Synchronize, settings->audio.synchronize);
  audio->set(Audio::Frequency, 48000u);
  if(!audio->init()) audio = Audio::create("None");

  input = Input::create(settings->input.driver);
  input->set(Input::Handle, presentation->viewport.handle());
  if(!input->init()) input = Input::create("None");

  dspaudio.setPrecision(16);
  dspaudio.setBalance(0.0);
  dspaudio.setFrequency(32040);
  dspaudio.setResampler(DSP::ResampleEngine::Sinc);
  dspaudio.setResamplerFrequency(48000);

  presentation->showSplash();

  if(loadMedia(location) == false) return;
  cpuDebugger->updateDisassembly();
  smpDebugger->updateDisassembly();
  memoryEditor->selectSource();
  propertiesViewer->updateProperties();
  vramViewer->updateTiles();
}

auto Program::main() -> void {
  debugger->run();
}

auto Program::quit() -> void {
  unloadMedia();
  windowManager->saveGeometry();
  settings->unload();
  Application::quit();
}
