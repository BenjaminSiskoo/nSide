auto R6502::op_store_absolute(uint8 reg) {
  abs.l = readPCi();
  abs.h = readPCi();
L write(abs.w, reg);
}

auto R6502::op_store_absolute_x(uint8& reg) {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.x);
L write(abs.w + r.x, reg);
}

auto R6502::op_store_absolute_y(uint8& reg) {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, reg);
}

auto R6502::op_store_zero_page(uint8 reg) {
  zp = readPCi();
L writeZP(zp, reg);
}

auto R6502::op_store_zero_page_x(uint8 reg) {
  zp = readPCi();
  readZP(zp);
L writeZP(zp + r.x, reg);
}

auto R6502::op_store_zero_page_y(uint8 reg) {
  zp = readPCi();
  readZP(zp);
L writeZP(zp + r.y, reg);
}

auto R6502::op_store_indirect_zero_page_x(uint8 reg) {
  zp = readPCi();
  readZP(zp);
  abs.l = readZP(zp++ + r.x);
  abs.h = readZP(zp++ + r.x);
L write(abs.w, reg);
}

auto R6502::op_store_indirect_zero_page_y(uint8& reg) {
  rd = readPCi();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  pageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, reg);
}
