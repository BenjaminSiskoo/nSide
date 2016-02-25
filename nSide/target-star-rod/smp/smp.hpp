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

  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;

  auto opcodeLength(uint16 addr) -> uint;
  auto updateDisassembly() -> void;
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

  auto loadRegisters() -> void;
  auto saveRegisters() -> void;
  SMPRegisterEditor();
};

extern unique_pointer<SMPDebugger> smpDebugger;
extern unique_pointer<SMPRegisterEditor> smpRegisterEditor;
