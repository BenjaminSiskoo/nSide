auto CPU::read(uint16 addr) -> uint8 {
  if(status.oam_dma_pending) {
    status.oam_dma_pending = false;
    read(addr);
    oam_dma();
  }

  while(status.rdy_line == 0) {
    r.mdr = bus.read(status.rdy_addr_valid ? status.rdy_addr_value : addr, r.mdr);
    addClocks(system.region() == System::Region::NTSC ? 12 : system.region() == System::Region::PAL ? 16 : 15);
  }

  r.mdr = bus.read(addr, r.mdr);
  addClocks(system.region() == System::Region::NTSC ? 12 : system.region() == System::Region::PAL ? 16 : 15);
  return r.mdr;
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  bus.write(addr, r.mdr = data);
  addClocks(system.region() == System::Region::NTSC ? 12 : system.region() == System::Region::PAL ? 16 : 15);
}

auto CPU::disassemblerRead(uint16 addr) -> uint8 {
  return bus.read(addr, r.mdr);
}
