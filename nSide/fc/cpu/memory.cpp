auto CPU::read(uint16 addr) -> uint8 {
  if(status.oamDMAPending) {
    status.oamDMAPending = false;
    read(addr);
    oamDMA();
  }

  while(status.rdyLine == 0) {
    r.mdr = bus.read(status.rdyAddressValid ? status.rdyAddressValue : addr, r.mdr);
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
