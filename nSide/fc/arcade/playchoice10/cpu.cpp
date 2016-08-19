auto PlayChoice10::PC10CPU::Enter() -> void {
  while(true) scheduler.synchronize(), playchoice10.pc10cpu.main();
}

auto PlayChoice10::PC10CPU::main() -> void {
  //interrupt_test();
  step(40);  //instruction();
}

auto PlayChoice10::PC10CPU::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(playchoice10.videoCircuit);
  synchronize(cpu);
}

auto PlayChoice10::PC10CPU::wait() -> void {
  print("PC10 CPU Wait\n");
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

auto PlayChoice10::PC10CPU::stop() -> bool {
}

auto PlayChoice10::PC10CPU::power() -> void {
  Z80::power();
}

auto PlayChoice10::PC10CPU::reset() -> void {
  create(PC10CPU::Enter, 4'000'000.0);
  Z80::reset();
}

auto PlayChoice10::PC10CPU::cycleEdge() -> void {
  print("PC10 CPU Cycle Edge\n");
  if(r.ei) {
    r.ei = false;
    r.ime = 1;
  }
}

auto PlayChoice10::PC10CPU::debuggerRead(uint16 addr) -> uint8 {
  return playchoice10.read(addr);
}
