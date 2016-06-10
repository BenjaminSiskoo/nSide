auto R6502::op_read_immediate(fp op) {
L rd = readPCi();
  call(op);
}

auto R6502::op_read_absolute(fp op) {
  abs.l = readPCi();
  abs.h = readPCi();
L rd = read(abs.w);
  call(op);
}

auto R6502::op_read_absolute_x(fp op) {
  abs.l = readPCi();
  abs.h = readPCi();
  page(abs.w, abs.w + r.x);
L rd = read(abs.w + r.x);
  call(op);
}

auto R6502::op_read_absolute_y(fp op) {
  abs.l = readPCi();
  abs.h = readPCi();
  page(abs.w, abs.w + r.y);
L rd = read(abs.w + r.y);
  call(op);
}

auto R6502::op_read_zero_page(fp op) {
  zp = readPCi();
L rd = readZP(zp);
  call(op);
}

auto R6502::op_read_zero_page_x(fp op) {
  zp = readPCi();
  readZP(zp);
L rd = readZP(zp + r.x);
  call(op);
}

auto R6502::op_read_zero_page_y(fp op) {
  zp = readPCi();
  readZP(zp);
L rd = readZP(zp + r.y);
  call(op);
}

auto R6502::op_read_indirect_zero_page_x(fp op) {
  zp = readPCi();
  readZP(zp);
  abs.l = readZP(zp++ + r.x);
  abs.h = readZP(zp++ + r.x);
L rd = read(abs.w);
  call(op);
}

auto R6502::op_read_indirect_zero_page_y(fp op) {
  rd = readPCi();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  page(abs.w, abs.w + r.y);
L rd = read(abs.w + r.y);
  call(op);
}
