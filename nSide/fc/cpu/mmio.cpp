auto CPU::readCPU(uint16 addr, uint8 data) -> uint8 {
  switch(addr) {

  case 0x4016: {
    if(system.vs()) return vssystem.read(side, addr, data);
    uint8 data = Famicom::peripherals.controllerPort1->data();
    data = data.bit(2) << 4 | data.bit(1) << 3 | data.bit(0) << 0;
    return (mdr() & 0xe0) | data | (Famicom::peripherals.expansionPort->data1() << 1);
  }

  case 0x4017: {
    if(system.vs()) return vssystem.read(side, addr, data);
    uint8 data = Famicom::peripherals.controllerPort2->data();
    data = data.bit(2) << 4 | data.bit(1) << 3 | data.bit(0) << 0;
    return (mdr() & 0xe0) | data | Famicom::peripherals.expansionPort->data2();
  }

  }

  if(system.vs() && addr >= 0x4018 && addr <= 0x5fff) return vssystem.read(side, addr, data);

  return apu.readIO(addr);
}

auto CPU::writeCPU(uint16 addr, uint8 data) -> void {
  switch(addr) {

  case 0x4014: {
    status.oamdmaPage = data;
    status.oamdmaPending = true;
    return;
  }

  case 0x4016: {
    if(system.vs()) return vssystem.write(side, addr, data);
    Famicom::peripherals.controllerPort1->latch(data.bit(0));
    Famicom::peripherals.controllerPort2->latch(data.bit(0));
    Famicom::peripherals.expansionPort->latch(data.bit(0));
    return;
  }

  }

  if(system.vs() && addr >= 0x4017 && addr <= 0x5fff) vssystem.write(side, addr, data);

  return apu.writeIO(addr, data);
}
