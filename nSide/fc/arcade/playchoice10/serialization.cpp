auto PlayChoice10::serialize(serializer& s) -> void {
  pc10cpu.serialize(s);

  s.array(wram);
  s.array(sram);

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
  s.integer(channel);
  s.integer(sramBank);

  s.array(videoCircuit.vram);
}

auto PlayChoice10::PC10CPU::serialize(serializer& s) -> void {
  Z80::serialize(s);
  Thread::serialize(s);
}
