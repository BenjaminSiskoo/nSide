auto R6502::readpc() -> uint8 {
  return read(regs.pc);
}

auto R6502::readpci() -> uint8 {
  return read(regs.pc++);
}

auto R6502::readsp() -> uint8 {
  return read(0x0100 | ++regs.s);
}

auto R6502::readzp(uint8 addr) -> uint8 {
  return read(addr);
}

//

auto R6502::writesp(uint8 data) -> void {
  write(0x0100 | regs.s--, data);
}

auto R6502::writezp(uint8 addr, uint8 data) -> void {
  write(addr, data);
}

//

auto R6502::page(uint16 x, uint16 y) -> void {
  if((x & 0xff00) != (y & 0xff00)) read((x & 0xff00) | (y & 0x00ff));
}

auto R6502::pageAlways(uint16 x, uint16 y) -> void {
  read((x & 0xff00) | (y & 0x00ff));
}
