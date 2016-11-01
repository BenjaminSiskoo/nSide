auto PlayChoice10::CPU::Enter() -> void {
  while(true) scheduler.synchronize(), playchoice10.pc10cpu.main();
}

auto PlayChoice10::CPU::main() -> void {
  instruction();
}

auto PlayChoice10::CPU::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(playchoice10.videoCircuit);
  synchronize(cpu);
}

auto PlayChoice10::CPU::power() -> void {
  Processor::Z80::bus = &playchoice10.pc10bus;
  Processor::Z80::power();
}

auto PlayChoice10::CPU::reset() -> void {
  Processor::Z80::reset();
  create(PlayChoice10::CPU::Enter, 4'000'000.0);
}
