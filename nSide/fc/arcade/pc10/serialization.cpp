auto PC10ArcadeBoard::serialize(serializer& s) -> void {
  s.array(wram);
  s.array(sram);
  s.array(vram);

  s.integer(dip);

  s.integer(vram_access);
  s.integer(controls);
  s.integer(ppu_output);
  s.integer(apu_output);
  s.integer(cpu_reset);
  s.integer(cpu_stop);
  s.integer(display);
  s.integer(z80_nmi);
  s.integer(watchdog);
  s.integer(ppu_reset);
  s.integer(channel);
  s.integer(sram_bank);
}
