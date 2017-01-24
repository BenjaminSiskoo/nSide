auto MOS6502::op_nop() -> void {
L idle();
}

auto MOS6502::op_brk() -> void {
  readPC();
  writeSP(r.pc >> 8);
  writeSP(r.pc >> 0);
  uint16 vector = 0xfffe;
  nmi(vector);
  writeSP(r.p | 0x30);
  abs.l = read(vector + 0);
  r.p.i = 1;
L abs.h = read(vector + 1);
  r.pc = abs.w;
}

auto MOS6502::op_set_flag(uint bit) -> void {
L idle();
  r.p |= 1 << bit;
}

auto MOS6502::op_clear_flag(uint bit) -> void {
L idle();
  r.p &= ~(1 << bit);
}

auto MOS6502::op_transfer(uint8& s, uint8& d, bool flag) -> void {
L idle();
  d = s;
  if(flag == false) return;
  r.p.n = (d & 0x80);
  r.p.z = (d == 0);
}

auto MOS6502::op_push(uint8& reg) -> void {
  idle();
L writeSP(reg);
}

auto MOS6502::op_php() -> void {
  idle();
L writeSP(r.p | 0x30);
}

auto MOS6502::op_pull(uint8& reg) -> void {
  idle();
  idle();
L reg = readSP();
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto MOS6502::op_plp() -> void {
  idle();
  idle();
L r.p = readSP();
}
