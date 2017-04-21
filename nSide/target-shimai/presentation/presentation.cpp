#include "../shimai.hpp"
unique_pointer<Presentation> presentation;

Presentation::Presentation() {
  presentation = this;

  viewport.setDroppable().onDrop([&](auto locations) {
    if(!directory::exists(locations(0))) return;
    program->mediumQueue.append(locations(0));
    program->loadMedium();
  });

  onClose([&] { program->quit(); });

  setTitle("shimai");
  setResizable(false);
  setBackgroundColor({0, 0, 0});
  resizeViewport();
  setCentered();

  #if defined(PLATFORM_WINDOWS)
  Application::Windows::onModalChange([](bool modal) { if(modal && audio) audio->clear(); });
  #endif

  #if defined(PLATFORM_MACOSX)
  showConfigurationSeparator.setVisible(false);
  showConfiguration.setVisible(false);
  about.setVisible(false);
  Application::Cocoa::onAbout([&] { about.doActivate(); });
  Application::Cocoa::onActivate([&] { setFocused(); });
  Application::Cocoa::onPreferences([&] { showConfiguration.doActivate(); });
  Application::Cocoa::onQuit([&] { doClose(); });
  #endif
}

auto Presentation::updateEmulator() -> void {
  if(!emulator) return;

  emulator->set("Blur Emulation", settings["Video/BlurEmulation"].boolean());
  emulator->set("Color Emulation", settings["Video/ColorEmulation"].boolean());
  emulator->set("Scanline Emulation", settings["Video/ScanlineEmulation"].boolean());
}

auto Presentation::clearViewport() -> void {
  if(!video) return;

  uint32_t* output;
  uint length = 0;
  uint width = viewport.geometry().width();
  uint height = viewport.geometry().height();
  if(video->lock(output, length, width, height)) {
    for(uint y : range(height)) {
      auto dp = output + y * (length >> 2);
      for(uint x : range(width)) *dp++ = 0xff000000;
    }

    video->unlock();
    video->refresh();
  }
}

auto Presentation::resizeViewport() -> void {
  //clear video area before resizing to avoid seeing distorted video momentarily
  clearViewport();

  uint scale = 2;
  if(settings["Video/Scale"].text() == "Tiny"  ) scale = 1;
  if(settings["Video/Scale"].text() == "Small" ) scale = 2;
  if(settings["Video/Scale"].text() == "Medium") scale = 3;
  if(settings["Video/Scale"].text() == "Large" ) scale = 4;

  uint windowWidth = 0, windowHeight = 0;
  bool aspectCorrection = true;
  bool integerScaling = true;
  if(!fullScreen()) {
    aspectCorrection = settings["Video/AspectCorrection"].boolean();
    windowWidth  = ceil(1280.0 * scale / 3.0);
    windowHeight = ceil( 720.0 * scale / 3.0);
  } else {
    integerScaling = settings["Video/Shader"].text() == "None";
    windowWidth  = geometry().width();
    windowHeight = geometry().height();
  }
  if(!fullScreen()) setSize({windowWidth, windowHeight});

  if(!emulator) {
    viewport.setGeometry({0, 0, windowWidth, windowHeight});
  } else {
    auto videoSize = emulator->videoSize(windowWidth, windowHeight, aspectCorrection, integerScaling);
    viewport.setGeometry({
      (windowWidth - videoSize.width) / 2, (windowHeight - videoSize.height) / 2,
      videoSize.width, videoSize.height
    });
  }

  //clear video area again to ensure entire viewport area has been painted in
  clearViewport();
}

auto Presentation::toggleFullScreen() -> void {
  if(!fullScreen()) {
    setResizable(true);
    setFullScreen(true);
    if(!input->acquired()) input->acquire();
  } else {
    if(input->acquired()) input->release();
    setFullScreen(false);
    setResizable(false);
  }
  resizeViewport();
}
