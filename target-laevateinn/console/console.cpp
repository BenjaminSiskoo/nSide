#include "../base.hpp"
ConsoleWindow* consoleWindow = nullptr;

#include "about.cpp"

ConsoleWindow::ConsoleWindow() {
  consoleWindow = this;
  setTitle({"Console - Laevateinn v", Emulator::Version});
  setGeometry({64, 640, 640, 400});
  setMenuVisible();

  menuEmulation.setText("&Emulation");
    menuEmulationReloadCartridge.setText("Reload Cartridge");
    menuEmulationPowerCycle.setText("Power Cycle");
    menuEmulationReset.setText("Reset");
    menuEmulationSynchronizeAudio.setText("Synchronize Audio");
    menuEmulationSynchronizeAudio.setChecked(settings->audio.synchronize);
    menuEmulationMuteAudio.setText("Mute Audio");
    menuEmulationMuteAudio.setChecked(settings->audio.mute);
    menuEmulation.append(menuEmulationReloadCartridge);
    menuEmulation.append(menuEmulationPowerCycle);
    menuEmulation.append(menuEmulationReset);
    menuEmulation.append(menuEmulationSeparator);
    menuEmulation.append(menuEmulationSynchronizeAudio);
    menuEmulation.append(menuEmulationMuteAudio);
  append(menuEmulation);

  menuDebug.setText("&Debug");
    menuDebugCPU.setText("CPU");
    menuDebugCPU.setChecked(debugger->debug.cpu);
    menuDebugSMP.setText("SMP");
    menuDebugSMP.setChecked(debugger->debug.smp);
    menuDebug.append(menuDebugCPU);
    menuDebug.append(menuDebugSMP);
  append(menuDebug);

  menuTracer.setText("&Tracer");
    menuTracerEnable.setText("Enable");
    menuTracerMask.setChecked(tracer->mask);
    menuTracerMask.setText("Mask");
    menuTracerMaskReset.setText("Reset Mask");
    menuTracer.append(menuTracerEnable);
    menuTracer.append(menuTracerMask);
    menuTracer.append(menuTracerMaskReset);
  append(menuTracer);

  menuWindows.setText("&Windows");
    menuWindowsPresentation.setText("Video");
    menuWindowsCPUDebugger.setText("CPU Debugger");
    menuWindowsSMPDebugger.setText("SMP Debugger");
    menuWindowsMemoryEditor.setText("Memory Editor");
    menuWindowsBreakpointEditor.setText("Breakpoint Editor");
    menuWindowsPropertiesViewer.setText("Properties Viewer");
    menuWindowsVRAMViewer.setText("VRAM Viewer");
    menuWindows.append(menuWindowsPresentation);
    menuWindows.append(menuWindowsSeparator1);
    menuWindows.append(menuWindowsCPUDebugger);
    menuWindows.append(menuWindowsSMPDebugger);
    menuWindows.append(menuWindowsSeparator2);
    menuWindows.append(menuWindowsMemoryEditor);
    menuWindows.append(menuWindowsBreakpointEditor);
    menuWindows.append(menuWindowsPropertiesViewer);
    menuWindows.append(menuWindowsVRAMViewer);
  append(menuWindows);

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
  menuState.append(menuStateSave1);
  menuState.append(menuStateSave2);
  menuState.append(menuStateSave3);
  menuState.append(menuStateSave4);
  menuState.append(menuStateSave5);
  menuState.append(menuStateSeparator);
  menuState.append(menuStateLoad1);
  menuState.append(menuStateLoad2);
  menuState.append(menuStateLoad3);
  menuState.append(menuStateLoad4);
  menuState.append(menuStateLoad5);
  append(menuState);

  menuHelp.setText("&Help");
    menuHelpAbout.setText("About ...");
    menuHelp.append(menuHelpAbout);
  append(menuHelp);

  layout.setMargin(5);
  runButton.setText("Run");
  stepButton.setText("Step");
  clearButton.setText("Clear");
  console.setFont(Font::monospace(8));

  layout.append(commandLayout, {~0, 0}, 5);
    commandLayout.append(runButton, {80, ~0}, 5);
    commandLayout.append(stepButton, {80, ~0}, 5);
    commandLayout.append(spacer, {~0, 0});
    commandLayout.append(clearButton, {80, ~0});
  layout.append(console, {~0, ~0});
  append(layout);

  onClose = [&] {
    setVisible(false);
    if(Intrinsics::platform() == Intrinsics::Platform::MacOSX) {
      interface->unload();
    } else {
      Application::quit();
    }
  };

  menuEmulationReloadCartridge.onActivate = [&] {
    interface->loadCartridge(interface->pathName);
  };

  menuEmulationPowerCycle.onActivate = [&] {
    emulator->power();
    print("System power cycled\n");
  };

  menuEmulationReset.onActivate = [&] {
    emulator->reset();
    print("System reset\n");
  };

  menuEmulationSynchronizeAudio.onToggle = [&] {
    audio->set(Audio::Synchronize, settings->audio.synchronize = menuEmulationSynchronizeAudio.checked());
  };

  menuEmulationMuteAudio.onToggle = [&] {
    settings->audio.mute = menuEmulationMuteAudio.checked();
  };

  menuDebugCPU.onToggle = [&] { debugger->debug.cpu = menuDebugCPU.checked(); };
  menuDebugSMP.onToggle = [&] { debugger->debug.smp = menuDebugSMP.checked(); };

  menuTracerEnable.onToggle = [&] { tracer->enable(menuTracerEnable.checked()); };
  menuTracerMask.onToggle = [&] { tracer->mask = menuTracerMask.checked(); };
  menuTracerMaskReset.onActivate = [&] {
    tracer->resetMask();
    debugger->print("Tracer mask reset\n");
  };

  menuWindowsPresentation.onActivate = [&] {
    presentation->setVisible();
    presentation->setFocused();
  };

  menuWindowsCPUDebugger.onActivate = [&] {
    cpuDebugger->setVisible();
    cpuDebugger->setFocused();
  };

  menuWindowsSMPDebugger.onActivate = [&] {
    smpDebugger->setVisible();
    smpDebugger->setFocused();
  };

  menuWindowsMemoryEditor.onActivate = [&] {
    memoryEditor->updateView();
    memoryEditor->setVisible();
    memoryEditor->setFocused();
  };

  menuWindowsBreakpointEditor.onActivate = [&] {
    breakpointEditor->setVisible();
    breakpointEditor->setFocused();
  };

  menuWindowsPropertiesViewer.onActivate = [&] {
    propertiesViewer->setVisible();
    propertiesViewer->setFocused();
  };

  menuWindowsVRAMViewer.onActivate = [&] {
    vramViewer->setVisible();
    vramViewer->setFocused();
  };

  menuStateSave1.onActivate = [&] { interface->saveState(1); };
  menuStateSave2.onActivate = [&] { interface->saveState(2); };
  menuStateSave3.onActivate = [&] { interface->saveState(3); };
  menuStateSave4.onActivate = [&] { interface->saveState(4); };
  menuStateSave5.onActivate = [&] { interface->saveState(5); };

  menuStateLoad1.onActivate = [&] { interface->loadState(1); };
  menuStateLoad2.onActivate = [&] { interface->loadState(2); };
  menuStateLoad3.onActivate = [&] { interface->loadState(3); };
  menuStateLoad4.onActivate = [&] { interface->loadState(4); };
  menuStateLoad5.onActivate = [&] { interface->loadState(5); };

  menuHelpAbout.onActivate = [&] { aboutWindow->setVisible(); };

  runButton.onActivate = [&] {
    if(debugger->paused) {
      print("\n");
      debugger->resume();
    } else {
      debugger->suspend();
    }
  };

  stepButton.onActivate = [&] {
    debugger->flags.step = true;
    debugger->resume();
  };

  clearButton.onActivate = [&] {
    console.setText("");
  };

  windowManager->append(this, "ConsoleWindow");
}

void ConsoleWindow::print(const string& text) {
  string output = console.text();
  output.append(text);
  console.setText(output);
  console.setCursorPosition(~0);
}
