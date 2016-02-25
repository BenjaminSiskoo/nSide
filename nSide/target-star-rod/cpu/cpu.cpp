#include "../laevateinn.hpp"
unique_pointer<CPUDebugger> cpuDebugger;

#include "registers.cpp"

auto CPUDebugger::mirror(uint24 addr) -> uint24 {
  if((addr & 0x40e000) == 0x0000) addr = 0x7e0000 | (addr & 0x1fff);  //$00-3f:80-bf:0000-1fff WRAM
  return addr;
}

auto CPUDebugger::read(uint24 addr) -> uint8 {
  if((addr & 0x40e000) == 0x2000) return ~0;  //$00-3f|80-bf:2000-3fff  MMIO
  if((addr & 0x40e000) == 0x4000) return ~0;  //$00-3f|80-bf:4000-5fff  MMIO
  return SuperFamicom::bus.read(mirror(addr), SuperFamicom::cpu.regs.mdr);
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

  uint length = lengthTable[SuperFamicom::bus.read(addr, SuperFamicom::cpu.regs.mdr)];
  if(length == M) return 3 - (SuperFamicom::cpu.regs.e | SuperFamicom::cpu.regs.p.m);
  if(length == X) return 3 - (SuperFamicom::cpu.regs.e | SuperFamicom::cpu.regs.p.x);
  return length;
  #undef M
  #undef X
}

auto CPUDebugger::updateDisassembly() -> void {
  string line[15];
  char text[512];

  SuperFamicom::cpu.disassemble_opcode(text, opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x);
  text[29] = 0;
  line[7] = { "> ", text };

  int addr = opcodePC;
  for(int o = 6; o >= 0; o--) {
    for(int b = 1; b <= 4; b++) {
      if(addr - b >= 0 && (debugger->cpuUsage.data[addr - b] & Usage::Exec)) {
        addr -= b;
        SuperFamicom::cpu.disassemble_opcode(text, addr, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x);
        text[29] = 0;
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
        SuperFamicom::cpu.disassemble_opcode(text, addr, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x);
        text[29] = 0;
        line[o] = { "  ", text };
        break;
      }
    }
  }

  string output;
  for(auto& n : line) {
    if(n.empty()) output.append("  ...\n");
    else output.append(n, "\n");
  }
  output.rtrim("\n");

  disassembly.setText(output);
  registers.setText({
     "A:", hex(SuperFamicom::cpu.regs.a, 4L), " X:", hex(SuperFamicom::cpu.regs.x, 4L), " Y:", hex(SuperFamicom::cpu.regs.y, 4L),
    " S:", hex(SuperFamicom::cpu.regs.s, 4L), " D:", hex(SuperFamicom::cpu.regs.d, 4L), " DB:", hex(SuperFamicom::cpu.regs.db, 2L), " ",
    SuperFamicom::cpu.regs.p.n ? "N" : "n", SuperFamicom::cpu.regs.p.v ? "V" : "v",
    SuperFamicom::cpu.regs.e ? (SuperFamicom::cpu.regs.p.m ? "1" : "0") : (SuperFamicom::cpu.regs.p.m ? "M" : "m"),
    SuperFamicom::cpu.regs.e ? (SuperFamicom::cpu.regs.p.x ? "B" : "b") : (SuperFamicom::cpu.regs.p.x ? "X" : "x"),
    SuperFamicom::cpu.regs.p.d ? "D" : "d", SuperFamicom::cpu.regs.p.i ? "I" : "i",
    SuperFamicom::cpu.regs.p.z ? "Z" : "z", SuperFamicom::cpu.regs.p.c ? "C" : "c",
  });
}

CPUDebugger::CPUDebugger() {
  cpuDebugger = this;
  opcodePC = 0x008000;

  setTitle("CPU Debugger");
  setGeometry({128, 128, 520, 255});

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

  windowManager->append(this, "CPUDebugger");
}
