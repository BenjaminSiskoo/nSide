auto CPU::read(uint16 addr) -> uint8 {
  if(io.oamdmaPending) {
    io.oamdmaPending = false;
    read(addr);
    oamdma();
  }

  while(io.rdyLine == 0) {
    r.mdr = bus.read(io.rdyAddrValid ? io.rdyAddrValue : addr, r.mdr);
    step(Region::NTSC() ? 12 : Region::PAL() ? 16 : 15);
  }

  r.mdr = bus.read(addr, r.mdr);
  step(Region::NTSC() ? 12 : Region::PAL() ? 16 : 15);
  return r.mdr;
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  bus.write(addr, r.mdr = data);
  step(Region::NTSC() ? 12 : Region::PAL() ? 16 : 15);
}

auto CPU::readDebugger(uint16 addr) -> uint8 {
  return bus.read(addr, r.mdr);
}
