auto PlayChoice10::PC10CPU::Enter() -> void {
  print("PC10 CPU Enter\n");
  while(true) scheduler.synchronize(), playchoice10.pc10cpu.main();
}

auto PlayChoice10::PC10CPU::main() -> void {
  //interrupt_test();
  //print("PC10 CPU main\n");
  step(system.colorburst());  //instruction();
}

auto PlayChoice10::PC10CPU::stop() -> bool {
}

auto PlayChoice10::PC10CPU::power() -> void {
  Z80::power();
}

auto PlayChoice10::PC10CPU::reset() -> void {
  create(PC10CPU::Enter, system.colorburst() * 6.0);
  Z80::reset();

  r[PC] = 0x0000;
  r[SP] = 0x0000;
  r[AF] = 0x0000;
  r[BC] = 0x0000;
  r[DE] = 0x0000;
  r[HL] = 0x0000;
}

auto PlayChoice10::PC10CPU::idle() -> void {
  print("PC10 CPU Idle\n");
  cycleEdge();
  step(2);
}

auto PlayChoice10::PC10CPU::read(uint16 addr) -> uint8 {
  print("PC10 CPU Read\n");
  cycleEdge();
  step(3);
  return playchoice10.read(addr);
}

auto PlayChoice10::PC10CPU::write(uint16 addr, uint8 data) -> void {
  print("PC10 CPU Write\n");
  cycleEdge();
  step(3);
  playchoice10.write(addr, data);
}

auto PlayChoice10::PC10CPU::portRead(uint8 port) -> uint8 {
  print("PC10 CPU Port Read\n");
  cycleEdge();
  step(4);
  return playchoice10.portRead(port);
}

auto PlayChoice10::PC10CPU::portWrite(uint8 port, uint8 data) -> void {
  print("PC10 CPU Port Write\n");
  cycleEdge();
  step(4);
  playchoice10.portWrite(port, data);
}

auto PlayChoice10::PC10CPU::cycleEdge() -> void {
  print("PC10 CPU Cycle Edge\n");
  if(r.ei) {
    r.ei = false;
    r.ime = 1;
  }
}

auto PlayChoice10::PC10CPU::step(uint clocks) -> void {
  print("PC10 CPU Step\n");
  Thread::step(clocks);
  synchronize(cpu);
}

auto PlayChoice10::PC10CPU::debuggerRead(uint16 addr) -> uint8 {
  return playchoice10.read(addr);
}
