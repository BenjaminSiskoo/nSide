#include "base.hpp"

Program* program = nullptr;
Video* video = nullptr;
Audio* audio = nullptr;
Input* input = nullptr;
DSP dspaudio;

string Program::path(string name) {
  string path = {basepath, name};
  if(file::exists(path) || directory::exists(path)) return path;
  path = {userpath, name};
  if(file::exists(path) || directory::exists(path)) return path;
  path = {sharedpath, name};
  if(file::exists(path) || directory::exists(path)) return path;
  return {userpath, name};
}

void Program::main() {
  debugger->run();
}

Program::Program(string pathname) {
  program = this;

  basepath = nall::programpath();
  userpath = {nall::configpath(), "laevateinn/"};
  sharedpath = {nall::sharedpath(), "higan/"};
  directory::create(userpath);

  settings = new Settings;
  new Interface;
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

  active = emulator;

  settings->load();

  string path = string::read({nall::configpath(), "higan/library.bml"}).strip().ltrim("Path: ").transform("\\", "/");
  if(path.empty()) path = {nall::userpath(), "Emulation/"};
  if(path.endsWith("/") == false) path.append("/");
  path.append("Super Famicom/");

  string foldername;
  if(pathname) foldername = pathname.transform("\\", "/").rtrim("/").append("/");
  if(!directory::exists(foldername)) foldername = BrowserWindow().setPath(path).directory();
  if(!foldername.endsWith(".sfc/")) return;
  if(!directory::exists(foldername)) return;

  windowManager->loadGeometry();
  consoleWindow->setVisible();
  presentation->setVisible();
  consoleWindow->setFocused();

  video = Video::create(settings->video.driver);
  video->set(Video::Handle, presentation->viewport.handle());
  if(!video->init()) {
    delete video;
    video = Video::create("None");
    video->init();
  }
  video->set(Video::Synchronize, settings->video.synchronize);
  video->set(Video::Filter, Video::FilterNearest);

  audio = Audio::create(settings->audio.driver);
  audio->set(Audio::Handle, presentation->viewport.handle());
  audio->set(Audio::Synchronize, settings->audio.synchronize);
  audio->set(Audio::Frequency, 48000u);
  if(!audio->init()) {
    delete audio;
    audio = Audio::create("None");
    audio->init();
  }

  input = Input::create(settings->input.driver);
  input->set(Input::Handle, presentation->viewport.handle());
  if(!input->init()) {
    delete input;
    input = Input::create("None");
    input->init();
  }

  dspaudio.setPrecision(16);
  dspaudio.setBalance(0.0);
  dspaudio.setFrequency(32000);
  dspaudio.setResampler(DSP::ResampleEngine::Hermite);
  dspaudio.setResamplerFrequency(48000);

  if(interface->loadCartridge(foldername) == false) return;
  cpuDebugger->updateDisassembly();
  smpDebugger->updateDisassembly();
  memoryEditor->selectSource();
  propertiesViewer->updateProperties();
  vramViewer->updateTiles();

  Application::main = {&Program::main, this};
  Application::run();

  interface->unload();
  windowManager->saveGeometry();
  settings->unload();
}

int main(int argc, char **argv) {
  #if defined(PLATFORM_WINDOWS)
  utf8_args(argc, argv);
  #endif

  Application::setName("Laevateinn");
  new Program(argv[1]);
  delete program;
  return 0;
}
