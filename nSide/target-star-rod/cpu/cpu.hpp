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
  VerticalLayout layout{this};
    HorizontalLayout primaryLayout{&layout, Size{~0, 0}, 5};
      Label    regALabel{&primaryLayout, Size{0, 0}, 5};
      LineEdit regAValue{&primaryLayout, Size{0, 0}, 5};
      Label    regXLabel{&primaryLayout, Size{0, 0}, 5};
      LineEdit regXValue{&primaryLayout, Size{0, 0}, 5};
      Label    regYLabel{&primaryLayout, Size{0, 0}, 5};
      LineEdit regYValue{&primaryLayout, Size{0, 0}, 5};
      Label    regSLabel{&primaryLayout, Size{0, 0}, 5};
      LineEdit regSValue{&primaryLayout, Size{0, 0}, 5};
      Label    regDLabel{&primaryLayout, Size{0, 0}, 5};
      LineEdit regDValue{&primaryLayout, Size{0, 0}, 5};
      Label    regBLabel{&primaryLayout, Size{0, 0}, 5};
      LineEdit regBValue{&primaryLayout, Size{0, 0}};
    HorizontalLayout secondaryLayout{&layout, Size{~0, 0}, 5};
      CheckLabel flagN{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagV{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagM{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagX{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagD{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagI{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagZ{&secondaryLayout, Size{0, 0}, 5};
      CheckLabel flagC{&secondaryLayout, Size{0, 0}};
    HorizontalLayout tertiaryLayout{&layout, Size{~0, 0}, 5};
      CheckLabel flagE{&tertiaryLayout, Size{~0, 0}, 5};
      Widget spacer{&tertiaryLayout, Size{0, 0}};
      Button update{&tertiaryLayout, Size{80, 0}};

  auto loadRegisters() -> void;
  auto saveRegisters() -> void;
  CPURegisterEditor();
};

extern unique_pointer<CPUDebugger> cpuDebugger;
extern unique_pointer<CPURegisterEditor> cpuRegisterEditor;
