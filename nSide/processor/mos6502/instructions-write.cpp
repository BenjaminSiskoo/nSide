auto MOS6502::op_store_absolute(uint8 reg) {
  abs.l = readPC();
  abs.h = readPC();
L write(abs.w, reg);
}

auto MOS6502::op_store_absolute_x(uint8& reg) {
  abs.l = readPC();
  abs.h = readPC();
  ioPageAlways(abs.w, abs.w + r.x);
L write(abs.w + r.x, reg);
}

auto MOS6502::op_store_absolute_y(uint8& reg) {
  abs.l = readPC();
  abs.h = readPC();
  ioPageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, reg);
}

auto MOS6502::op_store_zero_page(uint8 reg) {
  zp = readPC();
L writeZP(zp, reg);
}

auto MOS6502::op_store_zero_page_x(uint8 reg) {
  zp = readPC();
  readZP(zp);
L writeZP(zp + r.x, reg);
}

auto MOS6502::op_store_zero_page_y(uint8 reg) {
  zp = readPC();
  readZP(zp);
L writeZP(zp + r.y, reg);
}

auto MOS6502::op_store_indirect_zero_page_x(uint8 reg) {
  zp = readPC();
  readZP(zp);
  abs.l = readZP(zp++ + r.x);
  abs.h = readZP(zp++ + r.x);
L write(abs.w, reg);
}

auto MOS6502::op_store_indirect_zero_page_y(uint8& reg) {
  rd = readPC();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  ioPageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, reg);
}
