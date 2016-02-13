struct CPUDebugger : Window {
  uint24 opcodePC;

  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}, 5};
      Button stepInto{&controlLayout, Size{80, 0}, 5};
      Button stepNMI{&controlLayout, Size{40, 0}, 5};
      Button stepIRQ{&controlLayout, Size{40, 0}, 5};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}, 5};
      Button update{&controlLayout, Size{80, 0}};
    TextEdit disassembly{&layout, Size{~0, ~0}, 5};
    Button registers{&layout, Size{~0, 0}};

  uint24 mirror(uint24 addr);
  uint8 read(uint24 addr);
  void write(uint24 addr, uint8 data);

  uint opcodeLength(uint24 addr);
  void updateDisassembly();
  CPUDebugger();
};

struct CPURegisterEditor : Window {
  VerticalLayout layout;
    HorizontalLayout primaryLayout;
      Label regALabel;
      LineEdit regAValue;
      Label regXLabel;
      LineEdit regXValue;
      Label regYLabel;
      LineEdit regYValue;
      Label regSLabel;
      LineEdit regSValue;
      Label regDLabel;
      LineEdit regDValue;
      Label regDBLabel;
      LineEdit regDBValue;
    HorizontalLayout secondaryLayout;
      CheckLabel flagN;
      CheckLabel flagV;
      CheckLabel flagM;
      CheckLabel flagX;
      CheckLabel flagD;
      CheckLabel flagI;
      CheckLabel flagZ;
      CheckLabel flagC;
    HorizontalLayout tertiaryLayout;
      CheckLabel flagE;
      Widget spacer;
      Button update;

  void loadRegisters();
  void saveRegisters();
  CPURegisterEditor();
};

extern CPUDebugger* cpuDebugger;
extern CPURegisterEditor* cpuRegisterEditor;
