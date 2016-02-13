struct BreakpointEntry : HorizontalLayout {
  CheckLabel enable;
  LineEdit addr;
  LineEdit data;
  ComboButton type;
  ComboButton source;

  BreakpointEntry();
};

struct BreakpointEditor : Window {
  VerticalLayout layout{this};
  BreakpointEntry breakpointEntry[8];

  struct Breakpoint {
    enum : uint { Read, Write, Exec };
    enum : uint { CPU, SMP, VRAM, OAM, CGRAM };
    uint id;
    bool compare;
    uint addr;
    uint data;
    uint type;
    uint source;
  };
  vector<Breakpoint> breakpoint;
  vector<Breakpoint> breakpointReadCPU;
  vector<Breakpoint> breakpointWriteCPU;
  vector<Breakpoint> breakpointExecCPU;
  vector<Breakpoint> breakpointReadSMP;
  vector<Breakpoint> breakpointWriteSMP;
  vector<Breakpoint> breakpointExecSMP;
  vector<Breakpoint> breakpointReadVRAM;
  vector<Breakpoint> breakpointWriteVRAM;
  vector<Breakpoint> breakpointReadOAM;
  vector<Breakpoint> breakpointWriteOAM;
  vector<Breakpoint> breakpointReadCGRAM;
  vector<Breakpoint> breakpointWriteCGRAM;
  void synchronize();

  bool testReadCPU(uint24 addr);
  bool testWriteCPU(uint24 addr, uint8 data);
  bool testExecCPU(uint24 addr);

  bool testReadSMP(uint16 addr);
  bool testWriteSMP(uint16 addr, uint8 data);
  bool testExecSMP(uint16 addr);

  bool testReadVRAM(uint16 addr);
  bool testWriteVRAM(uint16 addr, uint8 data);

  bool testReadOAM(uint16 addr);
  bool testWriteOAM(uint16 addr, uint8 data);

  bool testReadCGRAM(uint16 addr);
  bool testWriteCGRAM(uint16 addr, uint8 data);

  BreakpointEditor();
};

extern BreakpointEditor* breakpointEditor;
