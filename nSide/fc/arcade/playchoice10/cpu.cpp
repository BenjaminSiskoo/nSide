auto PlayChoice10::CPU::Enter() -> void {
  while(true) scheduler.synchronize(), playchoice10.pc10cpu.main();
}

auto PlayChoice10::CPU::main() -> void {
  if(state.nmiLine) {
    state.nmiLine = 0;  //edge-sensitive
    irq(0, 0x0066, 0xff);
  }

  if(state.intLine) {
    //level-sensitive
    irq(1, 0x0038, 0xff);
  }

  instruction();
}

auto PlayChoice10::CPU::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(playchoice10.videoCircuit);
  synchronize(cpu0);
}

auto PlayChoice10::CPU::setNMI(bool value) -> void {
  state.nmiLine = value;
}

auto PlayChoice10::CPU::setINT(bool value) -> void {
  state.intLine = value;
}

auto PlayChoice10::CPU::power() -> void {
  Z80::bus = &playchoice10;
  Z80::power();
}

auto PlayChoice10::CPU::reset() -> void {
  Z80::reset();
  create(PlayChoice10::CPU::Enter, 4'000'000.0);

  memory::fill(&state, sizeof(State));
}
