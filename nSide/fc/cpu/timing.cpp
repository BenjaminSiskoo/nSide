auto CPU::addClocks(unsigned clocks) -> void {
  step(clocks);
}

auto CPU::lastCycle() -> void {
  status.interrupt_pending = ((status.irq_line | status.irq_apu_line) & ~regs.p.i) | status.nmi_pending;
}

auto CPU::nmi(uint16 &vector) -> void {
  if(status.nmi_pending) {
    status.nmi_pending = false;
    vector = 0xfffa;
  }
}

auto CPU::oam_dma() -> void {
  for(unsigned n = 0; n < 256; n++) {
    uint8 data = read((status.oam_dma_page << 8) + n);
    write(0x2004, data);
  }
}

auto CPU::set_nmi_line(bool line) -> void {
  //edge-sensitive (0->1)
  if(!status.nmi_line && line) status.nmi_pending = true;
  status.nmi_line = line;
}

auto CPU::set_irq_line(bool line) -> void {
  //level-sensitive
  status.irq_line = line;
}

auto CPU::set_irq_apu_line(bool line) -> void {
  //level-sensitive
  status.irq_apu_line = line;
}

auto CPU::set_rdy_line(bool line) -> void {
  status.rdy_line = line;
}

auto CPU::set_rdy_addr(bool valid, uint16 value) -> void {
  status.rdy_addr_valid = valid;
  status.rdy_addr_value = value;
}
