auto CPU::mmio_read(uint16 addr, uint8 data) -> uint8 {
  if(!system.vs()) {
    if(addr == 0x4016) {
      return (mdr() & 0xe0) | device.controllerPort1->data() | device.expansionPort->data1();
    }

    if(addr == 0x4017) {
      return (mdr() & 0xe0) | device.controllerPort2->data() | device.expansionPort->data2();
    }
  } else { // if using VS. System
    if(addr >= 0x4016 && addr <= 0x5fff) return vsarcadeboard.read(side, addr, data);
  }

  return apu.read(addr);
}

auto CPU::mmio_write(uint16 addr, uint8 data) -> void {
  if(addr == 0x4014) {
    status.oam_dma_page = data;
    status.oam_dma_pending = true;
  }

  if(!system.vs()) {
    if(addr == 0x4016) {
      device.controllerPort1->latch(data & 1);
      device.controllerPort2->latch(data & 1);
      device.expansionPort->latch(data & 1);
    }
  } else { // if using VS. System
    if(addr >= 0x4016 && addr <= 0x5fff) vsarcadeboard.write(side, addr, data);
  }

  return apu.write(addr, data);
}
