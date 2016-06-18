auto CPU::readCPU(uint16 addr, uint8 data) -> uint8 {
  if(!system.vs()) {
    if(addr == 0x4016) {
      return (mdr() & 0xe0) | Famicom::peripherals.controllerPort1->data() | (Famicom::peripherals.expansionPort->data1() << 1);
    }

    if(addr == 0x4017) {
      return (mdr() & 0xe0) | Famicom::peripherals.controllerPort2->data() | Famicom::peripherals.expansionPort->data2();
    }
  } else { // if using VS. System
    if(addr >= 0x4016 && addr <= 0x5fff) return vssystem.read(side, addr, data);
  }

  return apu.read(addr);
}

auto CPU::writeCPU(uint16 addr, uint8 data) -> void {
  if(addr == 0x4014) {
    status.oamDMAPage = data;
    status.oamDMAPending = true;
  }

  if(!system.vs()) {
    if(addr == 0x4016) {
      Famicom::peripherals.controllerPort1->latch(data & 1);
      Famicom::peripherals.controllerPort2->latch(data & 1);
      Famicom::peripherals.expansionPort->latch(data & 1);
    }
  } else { // if using VS. System
    if(addr >= 0x4016 && addr <= 0x5fff) vssystem.write(side, addr, data);
  }

  return apu.write(addr, data);
}
