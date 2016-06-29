auto R6502::op_increment(uint8& reg) {
L io();
  reg++;
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_decrement(uint8& reg) {
L io();
  reg--;
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_shift(fp op) {
L io();
  call(op);
}

auto R6502::op_rmw_absolute(fp op) {
  abs.l = readPC();
  abs.h = readPC();
  rd = read(abs.w);
  write(abs.w, rd);
  call(op);
L write(abs.w, rd);
}

auto R6502::op_rmw_absolute_x(fp op) {
  abs.l = readPC();
  abs.h = readPC();
  ioPageAlways(abs.w, abs.w + r.x);
  rd = read(abs.w + r.x);
  write(abs.w + r.x, rd);
  call(op);
L write(abs.w + r.x, rd);
}

auto R6502::op_rmw_zero_page(fp op) {
  zp = readPC();
  rd = readZP(zp);
  writeZP(zp, rd);
  call(op);
L writeZP(zp, rd);
}

auto R6502::op_rmw_zero_page_x(fp op) {
  zp = readPC();
  readZP(zp);
  rd = readZP(zp + r.x);
  writeZP(zp + r.x, rd);
  call(op);
L writeZP(zp + r.x, rd);
}
