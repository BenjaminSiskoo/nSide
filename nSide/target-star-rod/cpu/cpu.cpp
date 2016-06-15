#include "../laevateinn.hpp"
unique_pointer<CPUDebugger> cpuDebugger;

#include "registers.cpp"

CPUDebugger::CPUDebugger() {
  cpuDebugger = this;
  opcodePC = 0x008000;

  setTitle("CPU Debugger");
  setGeometry({128, 128, 620, 260});

  layout.setMargin(5);
  stepInto.setText("Step Into");
  stepNMI.setText("NMI");
  stepIRQ.setText("IRQ");
  autoUpdate.setText("Auto");
  update.setText("Update");
  disassembly.setFont(Font().setFamily(Font::Mono));
  registers.setFont(Font().setFamily(Font::Mono));
  registers.setText(" ");

  stepInto.onActivate([&] {
    debugger->flags.cpu.stepInto = true;
    debugger->resume();
  });

  stepNMI.onActivate([&] {
    debugger->flags.cpu.nmi = true;
    debugger->resume();
  });

  stepIRQ.onActivate([&] {
    debugger->flags.cpu.irq = true;
    debugger->resume();
  });

  update.onActivate({ &CPUDebugger::updateDisassembly, this });

  registers.onActivate([&] {
    cpuRegisterEditor->loadRegisters();
    cpuRegisterEditor->setVisible();
  });
}

auto CPUDebugger::mirror(uint24 addr) -> uint24 {
  if((addr & 0x40e000) == 0x0000) addr = 0x7e0000 | (addr & 0x1fff);  //$00-3f:80-bf:0000-1fff WRAM
  return addr;
}

auto CPUDebugger::read(uint24 addr) -> uint8 {
  if((addr & 0x40e000) == 0x2000) return ~0;  //$00-3f|80-bf:2000-3fff  MMIO
  if((addr & 0x40e000) == 0x4000) return ~0;  //$00-3f|80-bf:4000-5fff  MMIO
  return SuperFamicom::bus.read(mirror(addr), SuperFamicom::cpu.r.mdr);
}

auto CPUDebugger::write(uint24 addr, uint8 data) -> void {
  if((addr & 0x40e000) == 0x2000) return;  //$00-3f|80-bf:2000-3fff  MMIO
  if((addr & 0x40e000) == 0x4000) return;  //$00-3f|80-bf:4000-5fff  MMIO
  if((addr & 0x40e000) == 0x0000) addr = 0x7e0000 | (addr & 0x1fff);  //$00-3f:80-bf:0000-1fff WRAM
  return SuperFamicom::bus.write(mirror(addr), data);
}

auto CPUDebugger::opcodeLength(uint24 addr) -> uint {
  #define M 5
  #define X 6
  static uint lengthTable[256] = {
    2, 2, 2, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
    3, 2, 4, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,

    1, 2, 2, 2,  3, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  3, 2, 2, 2,  1, 3, 1, 1,  4, 3, 3, 4,
    1, 2, 3, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,

    2, 2, 3, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
    X, 2, X, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,

    X, 2, 2, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  2, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
    X, 2, 2, 2,  2, 2, 2, 2,  1, M, 1, 1,  3, 3, 3, 4,
    2, 2, 2, 2,  3, 2, 2, 2,  1, 3, 1, 1,  3, 3, 3, 4,
  };

  uint length = lengthTable[SuperFamicom::bus.read(addr, SuperFamicom::cpu.r.mdr)];
  if(length == M) return 3 - (SuperFamicom::cpu.r.e | SuperFamicom::cpu.r.p.m);
  if(length == X) return 3 - (SuperFamicom::cpu.r.e | SuperFamicom::cpu.r.p.x);
  return length;
  #undef M
  #undef X
}

auto CPUDebugger::updateDisassembly() -> void {
  string line[15];
  string text = SuperFamicom::cpu.disassemble(opcodePC, SuperFamicom::cpu.r.e, SuperFamicom::cpu.r.p.m, SuperFamicom::cpu.r.p.x);
  line[7] = { "> ", text };

  int addr = opcodePC;
  for(int o = 6; o >= 0; o--) {
    for(int b = 1; b <= 4; b++) {
      if(addr - b >= 0 && (debugger->cpuUsage.data[addr - b] & Usage::Exec)) {
        addr -= b;
        text = SuperFamicom::cpu.disassemble(addr, SuperFamicom::cpu.r.e, SuperFamicom::cpu.r.p.m, SuperFamicom::cpu.r.p.x);
        line[o] = { "  ", text };
        break;
      }
    }
  }

  addr = opcodePC;
  for(int o = 8; o <= 14; o++) {
    for(int b = 1; b <= 4; b++) {
      if(addr + b <= 0xffffff && (debugger->cpuUsage.data[addr + b] & Usage::Exec)) {
        addr += b;
        text = SuperFamicom::cpu.disassemble(addr, SuperFamicom::cpu.r.e, SuperFamicom::cpu.r.p.m, SuperFamicom::cpu.r.p.x);
        line[o] = { "  ", text };
        break;
      }
    }
  }

  string output;
  for(auto& n : line) {
    if(!n) output.append("  ...\n");
    else   output.append(n, "\n");
  }
  output.trimRight("\n");

  disassembly.setText(output);
  registers.setText({
     "A:", hex(SuperFamicom::cpu.r.a, 4L), " X:", hex(SuperFamicom::cpu.r.x, 4L), " Y:", hex(SuperFamicom::cpu.r.y, 4L),
    " S:", hex(SuperFamicom::cpu.r.s, 4L), " D:", hex(SuperFamicom::cpu.r.d, 4L), " DB:", hex(SuperFamicom::cpu.r.db, 2L), " ",
    SuperFamicom::cpu.r.p.n ? "N" : "n", SuperFamicom::cpu.r.p.v ? "V" : "v",
    SuperFamicom::cpu.r.e ? (SuperFamicom::cpu.r.p.m ? "1" : "0") : (SuperFamicom::cpu.r.p.m ? "M" : "m"),
    SuperFamicom::cpu.r.e ? (SuperFamicom::cpu.r.p.x ? "B" : "b") : (SuperFamicom::cpu.r.p.x ? "X" : "x"),
    SuperFamicom::cpu.r.p.d ? "D" : "d", SuperFamicom::cpu.r.p.i ? "I" : "i",
    SuperFamicom::cpu.r.p.z ? "Z" : "z", SuperFamicom::cpu.r.p.c ? "C" : "c",
  });
}
