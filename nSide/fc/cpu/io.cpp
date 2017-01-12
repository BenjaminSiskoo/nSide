auto CPU::readCPU(uint16 addr, uint8 data) -> uint8 {
  switch(addr) {

  case 0x4016: {
    uint8 data1 = Famicom::peripherals.controllerPort1->data();
    uint8 data2 = Famicom::peripherals.controllerPort2->mic() << 2;
    uint8 data3 = Famicom::peripherals.expansionPort->data1() << 1;
    data1 = data1.bit(2) << 4 | data1.bit(1) << 3 | data1.bit(0) << 0;
    return (mdr() & 0xe0) | data1 | data2 | data3;
  }

  case 0x4017: {
    uint8 data2 = Famicom::peripherals.controllerPort2->data();
    uint8 data3 = Famicom::peripherals.expansionPort->data2();
    data2 = data2.bit(2) << 4 | data2.bit(1) << 3 | data2.bit(0) << 0;
    return (mdr() & 0xe0)         | data2 | data3;
  }

  }

  return apu.readIO(addr);
}

auto CPU::writeCPU(uint16 addr, uint8 data) -> void {
  switch(addr) {

  case 0x4014: {
    io.oamdmaPage = data;
    io.oamdmaPending = true;
    return;
  }

  case 0x4016: {
    Famicom::peripherals.controllerPort1->latch(data.bit(0));
    Famicom::peripherals.controllerPort2->latch(data.bit(0));
    Famicom::peripherals.expansionPort->write(data.bits(0,2));
    return;
  }

  }

  return apu.writeIO(addr, data);
}
