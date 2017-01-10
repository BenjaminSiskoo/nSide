auto PlayChoice10::serialize(serializer& s) -> void {
  pc10bus.serialize(s);
  pc10cpu.serialize(s);

  s.integer(dip);

  s.integer(vramAccess);
  s.integer(controls);
  s.integer(ppuOutput);
  s.integer(apuOutput);
  s.integer(cpuReset);
  s.integer(cpuStop);
  s.integer(display);
  s.integer(z80NMI);
  s.integer(watchdog);
  s.integer(ppuReset);

  s.array(videoCircuit.vram);
}

auto PlayChoice10::Bus::serialize(serializer& s) -> void {
  s.integer(channel);
  s.integer(sramBank);

  s.array(wram);
  s.array(sram);
}

auto PlayChoice10::CPU::serialize(serializer& s) -> void {
  Processor::Z80::serialize(s);
  Thread::serialize(s);

  s.integer(state.nmiLine);
  s.integer(state.intLine);
}
