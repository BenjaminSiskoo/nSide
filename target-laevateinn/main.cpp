#include "base.hpp"

Program* program = nullptr;
Video* video = nullptr;
Audio* audio = nullptr;
Input* input = nullptr;
DSP dspaudio;

void Program::main() {
  debugger->run();
}

Program::Program(string pathname) {
  program = this;

  {
    char path[PATH_MAX];
    auto unused = ::realpath(argv[0], path);
    basepath = dir(path);
    unused = ::userpath(path);
    userpath = path;
    if(Intrinsics::platform() == Intrinsics::Platform::Windows) {
      userpath.append("laevateinn/");
    } else {
      userpath.append(".config/laevateinn/");
    }
    mkdir(userpath, 0755);
  }

  if(Intrinsics::platform() == Intrinsics::Platform::Windows) {
    proportionalFont = "Tahoma, 8";
    proportionalFontBold = "Tahoma, 8, Bold";
    monospaceFont = "Lucida Console, 8";
  } else {
    proportionalFont = "Sans, 8";
    proportionalFontBold = "Sans, 8, Bold";
    monospaceFont = "Liberation Mono, 8";
  }

  settings = new Settings;
  settings->load();

  string foldername;
  if(pathname) foldername = pathname.transform("\\", "/").rtrim("/").append("/");
  if(!directory::exists(foldername)) foldername = BrowserWindow().setPath(path).directory();
  if(!foldername.endsWith(".sfc/")) return;
  if(!directory::exists(foldername)) return;

  new Interface;
  new Debugger;
  new Tracer;
  new WindowManager;
  new ConsoleWindow;
  new AboutWindow;
  new VideoWindow;
  new CPUDebugger;
  new CPURegisterEditor;
  new SMPDebugger;
  new SMPRegisterEditor;
  new MemoryEditor;
  new BreakpointEditor;
  new PropertiesViewer;
  new VRAMViewer;

  windowManager->loadGeometry();
  consoleWindow->setVisible();
  videoWindow->setVisible();
  consoleWindow->setFocused();

  video = Video::create(settings->video.driver);
  video->set(Video::Handle, videoWindow->viewport.handle());
  if(!video->init()) {
    delete video;
    video = Video::create("None");
    video->init();
  }
  video->set(Video::Synchronize, settings->video.synchronize);
  video->set(Video::Filter, Video::FilterNearest);

  audio = Audio::create(settings->audio.driver);
  audio->set(Audio::Handle, videoWindow->viewport.handle());
  audio->set(Audio::Synchronize, settings->audio.synchronize);
  audio->set(Audio::Frequency, 48000u);
  if(audio->init() == false) {
    delete audio;
    audio = Audio::create("None");
    audio->init();
  }

  input = Input::create(settings->input.driver);
  input->set(Input::Handle, videoWindow->viewport.handle());
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

  interface->saveMemory();
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
