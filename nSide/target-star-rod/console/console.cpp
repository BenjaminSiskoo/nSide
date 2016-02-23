#include "../laevateinn.hpp"
ConsoleWindow* consoleWindow = nullptr;

#include "about.cpp"

ConsoleWindow::ConsoleWindow() {
  consoleWindow = this;
  setTitle({"Console - star-rod v", Emulator::Version});
  setGeometry({64, 640, 640, 400});

  menuEmulation.setText("&Emulation");
    menuEmulationReloadCartridge.setText("Reload Cartridge");
    menuEmulationPowerCycle.setText("Power Cycle");
    menuEmulationReset.setText("Reset");
    menuEmulationSynchronizeAudio.setText("Synchronize Audio");
    menuEmulationSynchronizeAudio.setChecked(settings->audio.synchronize);
    menuEmulationMuteAudio.setText("Mute Audio");
    menuEmulationMuteAudio.setChecked(settings->audio.mute);

  menuDebug.setText("&Debug");
    menuDebugCPU.setText("CPU");
    menuDebugCPU.setChecked(debugger->debug.cpu);
    menuDebugSMP.setText("SMP");
    menuDebugSMP.setChecked(debugger->debug.smp);

  menuTracer.setText("&Tracer");
    menuTracerEnable.setText("Enable");
    menuTracerMask.setChecked(tracer->mask);
    menuTracerMask.setText("Mask");
    menuTracerMaskReset.setText("Reset Mask");

  menuWindows.setText("&Windows");
    menuWindowsPresentation.setText("Video");
    menuWindowsCPUDebugger.setText("CPU Debugger");
    menuWindowsSMPDebugger.setText("SMP Debugger");
    menuWindowsMemoryEditor.setText("Memory Editor");
    menuWindowsBreakpointEditor.setText("Breakpoint Editor");
    menuWindowsPropertiesViewer.setText("Properties Viewer");
    menuWindowsVRAMViewer.setText("VRAM Viewer");

  menuState.setText("&State");
    menuStateSave1.setText("Save - Slot 1");
    menuStateSave2.setText("Save - Slot 2");
    menuStateSave3.setText("Save - Slot 3");
    menuStateSave4.setText("Save - Slot 4");
    menuStateSave5.setText("Save - Slot 5");
    menuStateLoad1.setText("Load - Slot 1");
    menuStateLoad2.setText("Load - Slot 2");
    menuStateLoad3.setText("Load - Slot 3");
    menuStateLoad4.setText("Load - Slot 4");
    menuStateLoad5.setText("Load - Slot 5");

  menuHelp.setText("&Help");
    menuHelpAbout.setText("About ...");

  layout.setMargin(5);
  runButton.setText("Run");
  stepButton.setText("Step");
  clearButton.setText("Clear");
  console.setFont(Font().setFamily(Font::Mono));

  onClose([&] {
    setVisible(false);
    if(Intrinsics::platform() == Intrinsics::Platform::MacOSX) {
      program->unloadMedia();
    } else {
      Application::quit();
    }
  });

  menuEmulationReloadCartridge.onActivate([&] {
    program->loadMedia(program->folderPaths(0));
  });

  menuEmulationPowerCycle.onActivate([&] {
    emulator->power();
    print("System power cycled\n");
  });

  menuEmulationReset.onActivate([&] {
    emulator->reset();
    print("System reset\n");
  });

  menuEmulationSynchronizeAudio.onToggle([&] {
    audio->set(Audio::Synchronize, settings->audio.synchronize = menuEmulationSynchronizeAudio.checked());
  });

  menuEmulationMuteAudio.onToggle([&] {
    settings->audio.mute = menuEmulationMuteAudio.checked();
  });

  menuDebugCPU.onToggle([&] { debugger->debug.cpu = menuDebugCPU.checked(); });
  menuDebugSMP.onToggle([&] { debugger->debug.smp = menuDebugSMP.checked(); });

  menuTracerEnable.onToggle([&] { tracer->enable(menuTracerEnable.checked()); });
  menuTracerMask.onToggle([&] { tracer->mask = menuTracerMask.checked(); });
  menuTracerMaskReset.onActivate([&] {
    tracer->resetMask();
    debugger->print("Tracer mask reset\n");
  });

  menuWindowsPresentation.onActivate([&] {
    presentation->setVisible();
    presentation->setFocused();
  });

  menuWindowsCPUDebugger.onActivate([&] {
    cpuDebugger->setVisible();
    cpuDebugger->setFocused();
  });

  menuWindowsSMPDebugger.onActivate([&] {
    smpDebugger->setVisible();
    smpDebugger->setFocused();
  });

  menuWindowsMemoryEditor.onActivate([&] {
    memoryEditor->updateView();
    memoryEditor->setVisible();
    memoryEditor->setFocused();
  });

  menuWindowsBreakpointEditor.onActivate([&] {
    breakpointEditor->setVisible();
    breakpointEditor->setFocused();
  });

  menuWindowsPropertiesViewer.onActivate([&] {
    propertiesViewer->setVisible();
    propertiesViewer->setFocused();
  });

  menuWindowsVRAMViewer.onActivate([&] {
    vramViewer->setVisible();
    vramViewer->setFocused();
  });

  menuStateSave1.onActivate([&] { program->saveState(1); });
  menuStateSave2.onActivate([&] { program->saveState(2); });
  menuStateSave3.onActivate([&] { program->saveState(3); });
  menuStateSave4.onActivate([&] { program->saveState(4); });
  menuStateSave5.onActivate([&] { program->saveState(5); });

  menuStateLoad1.onActivate([&] { program->loadState(1); });
  menuStateLoad2.onActivate([&] { program->loadState(2); });
  menuStateLoad3.onActivate([&] { program->loadState(3); });
  menuStateLoad4.onActivate([&] { program->loadState(4); });
  menuStateLoad5.onActivate([&] { program->loadState(5); });

  menuHelpAbout.onActivate([&] { aboutWindow->setVisible(); });

  runButton.onActivate([&] {
    if(debugger->paused) {
      print("\n");
      debugger->resume();
    } else {
      debugger->suspend();
    }
  });

  stepButton.onActivate([&] {
    debugger->flags.step = true;
    debugger->resume();
  });

  clearButton.onActivate([&] {
    console.setText("");
  });

  windowManager->append(this, "ConsoleWindow");
}

void ConsoleWindow::print(const string& text) {
  string output = console.text();
  output.append(text);
  console.setText(output);
  //console.setCursorPosition(~0);
}
