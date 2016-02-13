struct ConsoleWindow : Window {
  MenuBar menuBar{this};
    Menu menuEmulation{&menuBar};
      MenuItem menuEmulationReloadCartridge{&menuEmulation};
      MenuItem menuEmulationPowerCycle{&menuEmulation};
      MenuItem menuEmulationReset{&menuEmulation};
      MenuSeparator menuEmulationSeparator{&menuEmulation};
      MenuCheckItem menuEmulationSynchronizeAudio{&menuEmulation};
      MenuCheckItem menuEmulationMuteAudio{&menuEmulation};

    Menu menuDebug{&menuBar};
      MenuCheckItem menuDebugCPU{&menuDebug};
      MenuCheckItem menuDebugSMP{&menuDebug};

    Menu menuTracer{&menuBar};
      MenuCheckItem menuTracerEnable{&menuTracer};
      MenuCheckItem menuTracerMask{&menuTracer};
      MenuItem menuTracerMaskReset{&menuTracer};

    Menu menuWindows{&menuBar};
      MenuItem menuWindowsPresentation{&menuWindows};
      MenuSeparator menuWindowsSeparator1{&menuWindows};
      MenuItem menuWindowsCPUDebugger{&menuWindows};
      MenuItem menuWindowsSMPDebugger{&menuWindows};
      MenuSeparator menuWindowsSeparator2{&menuWindows};
      MenuItem menuWindowsMemoryEditor{&menuWindows};
      MenuItem menuWindowsBreakpointEditor{&menuWindows};
      MenuItem menuWindowsPropertiesViewer{&menuWindows};
      MenuItem menuWindowsVRAMViewer{&menuWindows};

    Menu menuState{&menuBar};
      MenuItem menuStateSave1{&menuState};
      MenuItem menuStateSave2{&menuState};
      MenuItem menuStateSave3{&menuState};
      MenuItem menuStateSave4{&menuState};
      MenuItem menuStateSave5{&menuState};
      MenuSeparator menuStateSeparator{&menuState};
      MenuItem menuStateLoad1{&menuState};
      MenuItem menuStateLoad2{&menuState};
      MenuItem menuStateLoad3{&menuState};
      MenuItem menuStateLoad4{&menuState};
      MenuItem menuStateLoad5{&menuState};

    Menu menuHelp{&menuBar};
      MenuItem menuHelpAbout{&menuHelp};

  VerticalLayout layout{this};
    HorizontalLayout commandLayout{&layout, Size{~0, 0}, 5};
      Button runButton{&commandLayout, Size{80, ~0}, 5};
      Button stepButton{&commandLayout, Size{80, ~0}, 5};
      Widget spacer{&commandLayout, Size{~0, 0}};
      Button clearButton{&commandLayout, Size{80, ~0}};
    TextEdit console{&layout, Size{~0, ~0}};

  void print(const string& text);

  ConsoleWindow();
};

struct AboutWindow : Window {
  VerticalLayout layout{this};
    Canvas canvas;
    Label title;
    Label version;
    Label website;

  AboutWindow();
};

extern ConsoleWindow* consoleWindow;
extern AboutWindow* aboutWindow;
