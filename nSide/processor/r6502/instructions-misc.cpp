auto R6502::op_nop() {
L io();
}

auto R6502::op_brk() {
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

auto R6502::op_set_flag(uint bit) {
L io();
  r.p |= 1 << bit;
}

auto R6502::op_clear_flag(uint bit) {
L io();
  r.p &= ~(1 << bit);
}

auto R6502::op_transfer(uint8& s, uint8& d, bool flag) {
L io();
  d = s;
  if(flag == false) return;
  r.p.n = (d & 0x80);
  r.p.z = (d == 0);
}

auto R6502::op_push(uint8& reg) {
  io();
L writeSP(reg);
}

auto R6502::op_php() {
  io();
L writeSP(r.p | 0x30);
}

auto R6502::op_pull(uint8& reg) {
  io();
  io();
L reg = readSP();
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_plp() {
  io();
  io();
L r.p = readSP();
}
