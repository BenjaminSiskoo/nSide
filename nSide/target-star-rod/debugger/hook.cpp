//S-CPU
//=====

auto Debugger::cpu_op_exec(uint24 addr) -> void {
  cpuUsage.data[addr] |= Usage::Exec;
  cpuDebugger->opcodePC = addr;
  bool breakpointHit = breakpointEditor->testExecCPU(addr);

  if((debug.cpu && tracer->enabled() && !tracer->maskCPU(addr))
  || (debug.cpu && flags.step)
  || flags.cpu.stepInto
  || breakpointHit
  ) {
    string text = SuperFamicom::cpu.disassemble(addr, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x);

    if(debug.cpu && tracer->enabled()) tracer->print(text, "\n");
    if((debug.cpu && flags.step) || flags.cpu.stepInto || breakpointHit) {
      print(text, "\n");
      if(debug.cpu && flags.step) {
        consoleWindow->stepButton.setFocused();
      }
      if(flags.cpu.stepInto) {
        cpuDebugger->stepInto.setFocused();
        cpuDebugger->updateDisassembly();
      }
      suspend();
      SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
    }
  }
}

auto Debugger::cpu_op_read(uint24 addr, uint8 data) -> void {
  cpuUsage.data[addr] |= Usage::Read;
  bool breakpointHit = breakpointEditor->testReadCPU(addr);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::cpu_op_write(uint24 addr, uint8 data) -> void {
  cpuUsage.data[addr] |= Usage::Write;
  bool breakpointHit = breakpointEditor->testWriteCPU(addr, data);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::cpu_op_nmi() -> void {
  if(flags.cpu.nmi) {
    print("CPU NMI\n");  //, text, "\n");
    flags.cpu.stepInto = true;
  }
}

auto Debugger::cpu_op_irq() -> void {
  if(flags.cpu.irq) {
    print("CPU IRQ\n");
    flags.cpu.stepInto = true;
  }
}

//S-SMP
//=====

auto Debugger::smp_op_exec(uint16 addr) -> void {
  apuUsage.data[addr] |= Usage::Exec;
  smpDebugger->opcodePC = addr;
  bool breakpointHit = breakpointEditor->testExecSMP(addr);

  if((debug.cpu && tracer->enabled() && !tracer->maskSMP(addr))
  || (debug.smp && flags.step)
  || flags.smp.stepInto
  || breakpointHit
  ) {
    string text = SuperFamicom::smp.disassemble(addr, SuperFamicom::smp.regs.p.p);

    if(debug.smp && tracer->enabled()) tracer->print(text, "\n");
    if((debug.smp && flags.step) || flags.smp.stepInto || breakpointHit) {
      print(text, "\n");
      if(debug.smp && flags.step) {
        consoleWindow->stepButton.setFocused();
      }
      if(flags.smp.stepInto) {
        smpDebugger->stepInto.setFocused();
        smpDebugger->updateDisassembly();
      }
      suspend();
      SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
    }
  }
}

auto Debugger::smp_op_read(uint16 addr, uint8 data) -> void {
  apuUsage.data[addr] |= Usage::Read;
  bool breakpointHit = breakpointEditor->testReadSMP(addr);

  if(breakpointHit) {
    print(SuperFamicom::smp.disassemble(smpDebugger->opcodePC, SuperFamicom::smp.regs.p.p), "\n");
    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::smp_op_write(uint16 addr, uint8 data) -> void {
  apuUsage.data[addr] |= Usage::Write;
  bool breakpointHit = breakpointEditor->testWriteSMP(addr, data);

  if(breakpointHit) {
    print(SuperFamicom::smp.disassemble(smpDebugger->opcodePC, SuperFamicom::smp.regs.p.p), "\n");
    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

//S-PPU
//=====

auto Debugger::ppu_vram_read(uint16 addr, uint8 data) -> void {
  bool breakpointHit = breakpointEditor->testReadVRAM(addr);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::ppu_vram_write(uint16 addr, uint8 data) -> void {
  bool breakpointHit = breakpointEditor->testWriteVRAM(addr, data);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::ppu_oam_read(uint16 addr, uint8 data) -> void {
  bool breakpointHit = breakpointEditor->testReadOAM(addr);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::ppu_oam_write(uint16 addr, uint8 data) -> void {
  bool breakpointHit = breakpointEditor->testWriteOAM(addr, data);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::ppu_cgram_read(uint16 addr, uint8 data) -> void {
  bool breakpointHit = breakpointEditor->testReadCGRAM(addr);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}

auto Debugger::ppu_cgram_write(uint16 addr, uint8 data) -> void {
  bool breakpointHit = breakpointEditor->testWriteCGRAM(addr, data);

  if(breakpointHit) {
    print(SuperFamicom::cpu.disassemble(cpuDebugger->opcodePC, SuperFamicom::cpu.regs.e, SuperFamicom::cpu.regs.p.m, SuperFamicom::cpu.regs.p.x), "\n");

    suspend();
    SuperFamicom::scheduler.exit(SuperFamicom::Scheduler::Event::Debugger);
  }
}
