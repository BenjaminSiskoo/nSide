struct SMPDebugger : Window {
  uint16 opcodePC;

  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}, 5};
      Button stepInto{&controlLayout, Size{80, 0}, 5};
      Widget spacer{&controlLayout, Size{~0, 0}};
      CheckLabel autoUpdate{&controlLayout, Size{0, 0}, 5};
      Button update{&controlLayout, Size{80, 0}};
    TextEdit disassembly{&layout, Size{~0, ~0}, 5};
    Button registers{&layout, Size{~0, 0}};

  uint8 read(uint16 addr);
  void write(uint16 addr, uint8 data);

  uint opcodeLength(uint16 addr);
  void updateDisassembly();
  SMPDebugger();
};

struct SMPRegisterEditor : Window {
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
    HorizontalLayout secondaryLayout;
      CheckLabel flagN;
      CheckLabel flagV;
      CheckLabel flagP;
      CheckLabel flagB;
      CheckLabel flagH;
      CheckLabel flagI;
      CheckLabel flagZ;
      CheckLabel flagC;
    HorizontalLayout tertiaryLayout;
      Widget spacer;
      Button update;

  void loadRegisters();
  void saveRegisters();
  SMPRegisterEditor();
};

extern SMPDebugger* smpDebugger;
extern SMPRegisterEditor* smpRegisterEditor;
