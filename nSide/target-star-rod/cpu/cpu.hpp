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

  auto mirror(uint24 addr) -> uint24;
  auto read(uint24 addr) -> uint8;
  auto write(uint24 addr, uint8 data) -> void;

  auto opcodeLength(uint24 addr) -> uint;
  auto updateDisassembly() -> void;
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

  auto loadRegisters() -> void;
  auto saveRegisters() -> void;
  CPURegisterEditor();
};

extern unique_pointer<CPUDebugger> cpuDebugger;
extern unique_pointer<CPURegisterEditor> cpuRegisterEditor;
