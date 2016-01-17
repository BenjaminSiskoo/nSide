#include "../laevateinn.hpp"
SMPDebugger* smpDebugger = nullptr;

#include "registers.cpp"

uint8 SMPDebugger::read(uint16 addr) {
  if((addr & 0xfff0) == 0x00f0) return ~0;  //$00f0-00ff  MMIO
  return SuperFamicom::smp.busRead(addr);
}

void SMPDebugger::write(uint16 addr, uint8 data) {
  if((addr & 0xfff0) == 0x00f0) return;  //$00f0-00ff  MMIO
  return SuperFamicom::smp.busWrite(addr, data);
}

uint SMPDebugger::opcodeLength(uint16 addr) {
  static uint lengthTable[256] = {
    0
  };
  return lengthTable[SuperFamicom::smp.busRead(addr)];
}

void SMPDebugger::updateDisassembly() {
  string line[15];

  line[7] = { "> ", SuperFamicom::smp.disassemble(opcodePC, SuperFamicom::smp.regs.p.p) };
  //line[7][31] = 0;

  int addr = opcodePC;
  for(int o = 6; o >= 0; o--) {
    for(int b = 1; b <= 3; b++) {
      if(addr - b >= 0 && (debugger->apuUsage.data[addr - b] & Usage::Exec)) {
        addr -= b;
        line[o] = { "  ", SuperFamicom::smp.disassemble(addr, SuperFamicom::smp.regs.p.p) };
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
        line[o] = { "  ", SuperFamicom::smp.disassemble(addr, SuperFamicom::smp.regs.p.p) };
        //line[o][31] = 0;
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
    "YA:", hex(SuperFamicom::smp.regs.y, 2L), hex(SuperFamicom::smp.regs.a, 2L),
    " A:", hex(SuperFamicom::smp.regs.a, 2L), " X:", hex(SuperFamicom::smp.regs.x, 2L),
    " Y:", hex(SuperFamicom::smp.regs.y, 2L), " S:01", hex(SuperFamicom::smp.regs.s, 2L), " ",
    SuperFamicom::smp.regs.p.n ? "N" : "n", SuperFamicom::smp.regs.p.v ? "V" : "v",
    SuperFamicom::smp.regs.p.p ? "P" : "p", SuperFamicom::smp.regs.p.b ? "B" : "b",
    SuperFamicom::smp.regs.p.h ? "H" : "h", SuperFamicom::smp.regs.p.i ? "I" : "i",
    SuperFamicom::smp.regs.p.z ? "Z" : "z", SuperFamicom::smp.regs.p.c ? "C" : "c",
  });
}

SMPDebugger::SMPDebugger() {
  smpDebugger = this;
  opcodePC = 0xffc0;

  setTitle("SMP Debugger");
  setGeometry({128, 128, 520, 255});

  layout.setMargin(5);
  stepInto.setText("Step Into");
  autoUpdate.setText("Auto");
  update.setText("Update");
  disassembly.setFont(Font::monospace(8));
  registers.setFont(Font::monospace(8));
  registers.setText(" ");

  layout.append(controlLayout, {~0, 0}, 5);
    controlLayout.append(stepInto, {80, 0}, 5);
    controlLayout.append(spacer, {~0, 0});
    controlLayout.append(autoUpdate, {0, 0}, 5);
    controlLayout.append(update, {80, 0});
  layout.append(disassembly, {~0, ~0}, 5);
  layout.append(registers, {~0, 0});
  append(layout);

  stepInto.onActivate = [&] {
    debugger->flags.smp.stepInto = true;
    debugger->resume();
  };

  update.onActivate = { &SMPDebugger::updateDisassembly, this };

  registers.onActivate = [&] {
    smpRegisterEditor->loadRegisters();
    smpRegisterEditor->setVisible();
  };

  windowManager->append(this, "SMPDebugger");
}
