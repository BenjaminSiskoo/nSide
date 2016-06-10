auto R6502::op_nop() {
L readPC();
}

auto R6502::op_brk() {
  readPCi();
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
L readPC();
  r.p |= 1 << bit;
}

auto R6502::op_clear_flag(uint bit) {
L readPC();
  r.p &= ~(1 << bit);
}

auto R6502::op_transfer(uint8& s, uint8& d, bool flag) {
L readPC();
  d = s;
  if(flag == false) return;
  r.p.n = (d & 0x80);
  r.p.z = (d == 0);
}

auto R6502::op_push(uint8& reg) {
  readPC();
L writeSP(reg);
}

auto R6502::op_php() {
  readPC();
L writeSP(r.p | 0x30);
}

auto R6502::op_pull(uint8& reg) {
  readPC();
  readPC();
L reg = readSP();
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_plp() {
  readPC();
  readPC();
L r.p = readSP();
}
