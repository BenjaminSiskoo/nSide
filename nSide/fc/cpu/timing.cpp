auto CPU::addClocks(uint clocks) -> void {
  step(clocks);
}

auto CPU::lastCycle() -> void {
  status.interruptPending = ((status.irqLine | status.irqAPULine) & ~r.p.i) | status.nmiPending;
}

auto CPU::nmi(uint16 &vector) -> void {
  if(status.nmiPending) {
    status.nmiPending = false;
    vector = 0xfffa;
  }
}

auto CPU::oamDMA() -> void {
  for(uint n = 0; n < 256; n++) {
    uint8 data = read((status.oamDMAPage << 8) + n);
    write(0x2004, data);
  }
}

auto CPU::setNMILine(bool line) -> void {
  //edge-sensitive (0->1)
  if(!status.nmiLine && line) status.nmiPending = true;
  status.nmiLine = line;
}

auto CPU::setIRQLine(bool line) -> void {
  //level-sensitive
  status.irqLine = line;
}

auto CPU::setIRQAPULine(bool line) -> void {
  //level-sensitive
  status.irqAPULine = line;
}

auto CPU::setRDYLine(bool line) -> void {
  status.rdyLine = line;
}

auto CPU::setRDYAddress(bool valid, uint16 value) -> void {
  status.rdyAddressValid = valid;
  status.rdyAddressValue = value;
}
