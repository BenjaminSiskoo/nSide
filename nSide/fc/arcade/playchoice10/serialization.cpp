auto PlayChoice10::serialize(serializer& s) -> void {
  s.array(wram);
  s.array(sram);
  s.array(vram);

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
}
