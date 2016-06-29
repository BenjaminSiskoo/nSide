#include "../laevateinn.hpp"
unique_pointer<SMPDebugger> smpDebugger;

#include "registers.cpp"

SMPDebugger::SMPDebugger() {
  smpDebugger = this;
  opcodePC = 0xffc0;

  setTitle("SMP Debugger");
  setGeometry({128, 128, 520, 260});

  layout.setMargin(5);
  stepInto.setText("Step Into");
  autoUpdate.setText("Auto");
  update.setText("Update");
  disassembly.setFont(Font().setFamily(Font::Mono));
  registers.setFont(Font().setFamily(Font::Mono));
  registers.setText(" ");

  stepInto.onActivate([&] {
    debugger->flags.smp.stepInto = true;
    debugger->resume();
  });

  update.onActivate({ &SMPDebugger::updateDisassembly, this });

  registers.onActivate([&] {
    smpRegisterEditor->loadRegisters();
    smpRegisterEditor->setVisible();
  });
}

auto SMPDebugger::read(uint16 addr) -> uint8 {
  if((addr & 0xfff0) == 0x00f0) return ~0;  //$00f0-00ff  MMIO
  return SFC::smp.readBus(addr);
}

auto SMPDebugger::write(uint16 addr, uint8 data) -> void {
  if((addr & 0xfff0) == 0x00f0) return;  //$00f0-00ff  MMIO
  return SFC::smp.writeBus(addr, data);
}

auto SMPDebugger::opcodeLength(uint16 addr) -> uint {
  static uint lengthTable[256] = {
    0
  };
  return lengthTable[SFC::smp.readBus(addr)];
}

auto SMPDebugger::updateDisassembly() -> void {
  string line[15];

  line[7] = { "> ", SFC::smp.disassemble(opcodePC, SFC::smp.regs.p.p) };
  //line[7][31] = 0;

  int addr = opcodePC;
  for(int o = 6; o >= 0; o--) {
    for(int b = 1; b <= 3; b++) {
      if(addr - b >= 0 && (debugger->apuUsage.data[addr - b] & Usage::Exec)) {
        addr -= b;
        line[o] = { "  ", SFC::smp.disassemble(addr, SFC::smp.regs.p.p) };
        //line[o][31] = 0;
        break;
      }
    }
  }

  addr = opcodePC;
  for(int o = 8; o <= 14; o++) {
    for(int b = 1; b <= 3; b++) {
      if(addr - b <= 0xffff && (debugger->apuUsage.data[addr + b] & Usage::Exec)) {
        addr += b;
        line[o] = { "  ", SFC::smp.disassemble(addr, SFC::smp.regs.p.p) };
        //line[o][31] = 0;
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
    "YA:", hex(SFC::smp.regs.y, 2L), hex(SFC::smp.regs.a, 2L),
    " A:", hex(SFC::smp.regs.a, 2L), " X:", hex(SFC::smp.regs.x, 2L),
    " Y:", hex(SFC::smp.regs.y, 2L), " S:01", hex(SFC::smp.regs.s, 2L), " ",
    SFC::smp.regs.p.n ? "N" : "n", SFC::smp.regs.p.v ? "V" : "v",
    SFC::smp.regs.p.p ? "P" : "p", SFC::smp.regs.p.b ? "B" : "b",
    SFC::smp.regs.p.h ? "H" : "h", SFC::smp.regs.p.i ? "I" : "i",
    SFC::smp.regs.p.z ? "Z" : "z", SFC::smp.regs.p.c ? "C" : "c",
  });
}
