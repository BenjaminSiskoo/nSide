auto CPU::serialize(serializer& s) -> void {
  R6502::serialize(s);
  Thread::serialize(s);

  s.array(ram);

  s.integer(status.interrupt_pending);
  s.integer(status.nmi_pending);
  s.integer(status.nmi_line);
  s.integer(status.irq_line);
  s.integer(status.irq_apu_line);

  s.integer(status.rdy_line);
  s.integer(status.rdy_addr_valid);
  s.integer(status.rdy_addr_value);

  s.integer(status.oam_dma_pending);
  s.integer(status.oam_dma_page);
}
