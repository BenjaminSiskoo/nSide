auto MOS6502::op_read_immediate(fp op) -> void {
L rd = readPC();
  call(op);
}

auto MOS6502::op_read_absolute(fp op) -> void {
  abs.l = readPC();
  abs.h = readPC();
L rd = read(abs.w);
  call(op);
}

auto MOS6502::op_read_absolute_x(fp op) -> void {
  abs.l = readPC();
  abs.h = readPC();
  ioPage(abs.w, abs.w + r.x);
L rd = read(abs.w + r.x);
  call(op);
}

auto MOS6502::op_read_absolute_y(fp op) -> void {
  abs.l = readPC();
  abs.h = readPC();
  ioPage(abs.w, abs.w + r.y);
L rd = read(abs.w + r.y);
  call(op);
}

auto MOS6502::op_read_zero_page(fp op) -> void {
  zp = readPC();
L rd = readZP(zp);
  call(op);
}

auto MOS6502::op_read_zero_page_x(fp op) -> void {
  zp = readPC();
  readZP(zp);
L rd = readZP(zp + r.x);
  call(op);
}

auto MOS6502::op_read_zero_page_y(fp op) -> void {
  zp = readPC();
  readZP(zp);
L rd = readZP(zp + r.y);
  call(op);
}

auto MOS6502::op_read_indirect_zero_page_x(fp op) -> void {
  zp = readPC();
  readZP(zp);
  abs.l = readZP(zp++ + r.x);
  abs.h = readZP(zp++ + r.x);
L rd = read(abs.w);
  call(op);
}

auto MOS6502::op_read_indirect_zero_page_y(fp op) -> void {
  rd = readPC();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  ioPage(abs.w, abs.w + r.y);
L rd = read(abs.w + r.y);
  call(op);
}
