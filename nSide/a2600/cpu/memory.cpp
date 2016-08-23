auto CPU::read(uint16 addr) -> uint8 {
  addr &= 0x1fff;

  do {
    if(io.rdyLine == 0 && io.rdyAddrValid) addr = io.rdyAddrValue;
    if((addr & 0x1080) == 0x0000) r.mdr = tia.readIO(addr, r.mdr);
    if((addr & 0x1280) == 0x0080) r.mdr = pia.readRAM(addr, r.mdr);
    if((addr & 0x1280) == 0x0280) r.mdr = pia.readIO(addr, r.mdr);
    if((addr & 0x1000) == 0x1000) r.mdr = cartridge.access(addr, r.mdr);
    step(3);
  } while(io.rdyLine == 0);

  return r.mdr;
}

auto CPU::write(uint16 addr, uint8 data) -> void {
  addr &= 0x1fff;
  r.mdr = data;
  if((addr & 0x1080) == 0x0000) tia.writeIO(addr, data);
  if((addr & 0x1280) == 0x0080) pia.writeRAM(addr, data);
  if((addr & 0x1280) == 0x0280) pia.writeIO(addr, data);
  if((addr & 0x1000) == 0x1000) cartridge.access(addr, data);
  step(3);
}