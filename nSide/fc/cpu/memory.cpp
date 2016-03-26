auto CPU::read(uint16 addr) -> uint8 {
  if(status.oam_dma_pending) {
    status.oam_dma_pending = false;
    read(addr);
    oam_dma();
  }

  while(status.rdy_line == 0) {
    regs.mdr = bus.read(status.rdy_addr_valid ? status.rdy_addr_value : addr, regs.mdr);
    addClocks(system.region() == System::Region::NTSC ? 12 : 16);
  }

  regs.mdr = bus.read(addr, regs.mdr);
  addClocks(system.region() == System::Region::NTSC ? 12 : 16);
  return regs.mdr;
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  bus.write(addr, regs.mdr = data);
  addClocks(system.region() == System::Region::NTSC ? 12 : 16);
}

auto CPU::disassemblerRead(uint16 addr) -> uint8 {
  return bus.read(addr, regs.mdr);
}
