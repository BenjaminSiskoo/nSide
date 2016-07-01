auto CPU::step(uint clocks) -> void {
  apu.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizeAPU();

  ppu.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizePPU();

  cartridge.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizeCartridge();

  if(system.vs()) vssystem.clock -= clocks;
  if(!scheduler.synchronizing()) synchronizeCoprocessors();

  for(auto peripheral : peripherals) {
    peripheral->clock -= clocks * (uint64)peripheral->frequency;
  }
  synchronizePeripherals();
}

auto CPU::lastCycle() -> void {
  status.interruptPending = ((status.irqLine | status.apuLine) & ~r.p.i) | status.nmiPending;
}

auto CPU::nmi(uint16 &vector) -> void {
  if(status.nmiPending) {
    status.nmiPending = false;
    vector = 0xfffa;
  }
}

auto CPU::oamdma() -> void {
  for(uint n = 0; n < 256; n++) {
    uint8 data = read((io.oamdmaPage << 8) + n);
    write(0x2004, data);
  }
}

auto CPU::nmiLine(bool line) -> void {
  //edge-sensitive (0->1)
  if(!status.nmiLine && line) status.nmiPending = true;
  status.nmiLine = line;
}

auto CPU::irqLine(bool line) -> void {
  //level-sensitive
  status.irqLine = line;
}

auto CPU::apuLine(bool line) -> void {
  //level-sensitive
  status.apuLine = line;
}

auto CPU::rdyLine(bool line) -> void {
  status.rdyLine = line;
}

auto CPU::rdyAddr(bool valid, uint16 value) -> void {
  status.rdyAddrValid = valid;
  status.rdyAddrValue = value;
}
