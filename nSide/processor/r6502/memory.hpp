alwaysinline auto readPC() -> uint8 {
  return read(r.pc);
}

alwaysinline auto readPCi() -> uint8 {
  return read(r.pc++);
}

alwaysinline auto readSP() -> uint8 {
  return read(0x0100 | ++r.s);
}

alwaysinline auto readZP(uint8 addr) -> uint8 {
  return read(addr);
}

alwaysinline auto writeSP(uint8 data) -> void {
  write(0x0100 | r.s--, data);
}

alwaysinline auto writeZP(uint8 addr, uint8 data) -> void {
  write(addr, data);
}

alwaysinline auto page(uint16 x, uint16 y) -> void {
  if((x & 0xff00) != (y & 0xff00)) read((x & 0xff00) | (y & 0x00ff));
}

alwaysinline auto pageAlways(uint16 x, uint16 y) -> void {
  read((x & 0xff00) | (y & 0x00ff));
}
