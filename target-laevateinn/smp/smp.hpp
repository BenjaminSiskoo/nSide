struct SMPDebugger : Window {
  uint16 opcodePC;

  VerticalLayout layout;
    HorizontalLayout controlLayout;
      Button stepInto;
      Widget spacer;
      CheckLabel autoUpdate;
      Button update;
    TextEdit disassembly;
    Button registers;

  uint8 read(uint16 addr);
  void write(uint16 addr, uint8 data);

  unsigned opcodeLength(uint16 addr);
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
