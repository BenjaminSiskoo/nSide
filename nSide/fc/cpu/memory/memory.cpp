auto CPU::op_read(uint16 addr) -> uint8 {
  if(status.oam_dma_pending) {
    status.oam_dma_pending = false;
    op_read(addr);
    oam_dma();
  }

  while(status.rdy_line == 0) {
    regs.mdr = bus.read(status.rdy_addr_valid ? status.rdy_addr_value : addr, regs.mdr);
    add_clocks(system.region() == System::Region::NTSC ? 12 : 16);
  }

  regs.mdr = bus.read(addr, regs.mdr);
  add_clocks(system.region() == System::Region::NTSC ? 12 : 16);
  return regs.mdr;
}

auto CPU::op_write(uint16 addr, uint8 data) -> void {
  bus.write(addr, regs.mdr = data);
  add_clocks(system.region() == System::Region::NTSC ? 12 : 16);
}

auto CPU::disassembler_read(uint16 addr) -> uint8 {
  return bus.read(addr, regs.mdr);
}
