//opcode implementations
//======================

auto R6502::op_branch(bool condition) {
  if(condition == false) {
L   rd = readPCi();
  } else {
    rd = readPCi();
    aa = r.pc + (int8)rd;
    page(r.pc, aa);
L   readPC();
    r.pc = aa;
  }
}

auto R6502::op_clear_flag(bool& flag) {
L readPC();
  flag = 0;
}

auto R6502::op_decrement(uint8& reg) {
L readPC();
  reg--;
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_increment(uint8& reg) {
L readPC();
  reg++;
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_pull(uint8& reg) {
  readPC();
  readPC();
L reg = readSP();
  r.p.n = (reg & 0x80);
  r.p.z = (reg == 0);
}

auto R6502::op_push(uint8& reg) {
  readPC();
L writeSP(reg);
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

auto R6502::op_read_immediate(fp op) {
L rd = readPCi();
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

auto R6502::op_rmw_absolute(fp op) {
  abs.l = readPCi();
  abs.h = readPCi();
  rd = read(abs.w);
  write(abs.w, rd);
  call(op);
L write(abs.w, rd);
}

auto R6502::op_rmw_absolute_x(fp op) {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.x);
  rd = read(abs.w + r.x);
  write(abs.w + r.x, rd);
  call(op);
L write(abs.w + r.x, rd);
}

auto R6502::op_rmw_zero_page(fp op) {
  zp = readPCi();
  rd = readZP(zp);
  writeZP(zp, rd);
  call(op);
L writeZP(zp, rd);
}

auto R6502::op_rmw_zero_page_x(fp op) {
  zp = readPCi();
  readZP(zp);
  rd = readZP(zp + r.x);
  writeZP(zp + r.x, rd);
  call(op);
L writeZP(zp + r.x, rd);
}

auto R6502::op_set_flag(bool& flag) {
L readPC();
  flag = 1;
}

auto R6502::op_shift(fp op) {
L readPC();
  call(op);
}

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

auto R6502::op_transfer(uint8& s, uint8& d, bool flag) {
L readPC();
  d = s;
  if(flag == false) return;
  r.p.n = (d & 0x80);
  r.p.z = (d == 0);
}

//opcodes
//=======

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

auto R6502::op_jmp_absolute() {
  abs.l = readPCi();
L abs.h = readPCi();
  r.pc = abs.w;
}

auto R6502::op_jmp_indirect_absolute() {
  abs.l = readPCi();
  abs.h = readPCi();
  iabs.l = read(abs.w); abs.l++;
L iabs.h = read(abs.w); abs.l++;
  r.pc = iabs.w;
}

auto R6502::op_jsr_absolute() {
  abs.l = readPCi();
  abs.h = readPCi();
  readPC();
  r.pc--;
  writeSP(r.pc >> 8);
L writeSP(r.pc >> 0);
  r.pc = abs.w;
}

auto R6502::op_nop() {
L readPC();
}

auto R6502::op_php() {
  readPC();
L writeSP(r.p | 0x30);
}

auto R6502::op_plp() {
  readPC();
  readPC();
L r.p = readSP();
}

auto R6502::op_rti() {
  readPC();
  readPC();
  r.p = readSP();
  abs.l = readSP();
L abs.h = readSP();
  r.pc = abs.w;
}

auto R6502::op_rts() {
  readPC();
  readPC();
  abs.l = readSP();
  abs.h = readSP();
L readPC();
  r.pc = ++abs.w;
}

//illegal opcode implementations
//==============================

auto R6502::opill_rmwr_absolute(fp opw, fp opr) {
  abs.l = readPCi();
  abs.h = readPCi();
  rd = read(abs.w);
  write(abs.w, rd);
  call(opw);
L write(abs.w, rd);
  call(opr);
}

auto R6502::opill_rmwr_absolute_x(fp opw, fp opr) {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.x);
  rd = read(abs.w + r.x);
  write(abs.w + r.x, rd);
  call(opw);
L write(abs.w + r.x, rd);
  call(opr);
}

auto R6502::opill_rmwr_absolute_y(fp opw, fp opr) {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.y);
  rd = read(abs.w + r.y);
  write(abs.w + r.y, rd);
  call(opw);
L write(abs.w + r.y, rd);
  call(opr);
}

auto R6502::opill_rmwr_indirect_zero_page_x(fp opw, fp opr) {
  zp = readPCi();
  readZP(zp);
  abs.l = readZP(zp++ + r.x);
  abs.h = readZP(zp++ + r.x);
  rd = read(abs.w);
  write(abs.w, rd);
  call(opw);
L write(abs.w, rd);
  call(opr);
}

auto R6502::opill_rmwr_indirect_zero_page_y(fp opw, fp opr) {
  rd = readPCi();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  pageAlways(abs.w, abs.w + r.y);
  rd = read(abs.w + r.y);
  write(abs.w + r.y, rd);
  call(opw);
L write(abs.w + r.y, rd);
  call(opr);
}

auto R6502::opill_rmwr_zero_page(fp opw, fp opr) {
  zp = readPCi();
  rd = readZP(zp);
  writeZP(zp, rd);
  call(opw);
L writeZP(zp, rd);
  call(opr);
}

auto R6502::opill_rmwr_zero_page_x(fp opw, fp opr) {
  zp = readPCi();
  readZP(zp);
  rd = readZP(zp + r.x);
  writeZP(zp + r.x, rd);
  call(opw);
L writeZP(zp + r.x, rd);
  call(opr);
}

//illegal opcodes
//===============

auto R6502::opill_alr_immediate() {
  op_read_immediate(&R6502::op_and);
  op_sra();
}

auto R6502::opill_anc_immediate() {
L rd = readPCi();
  op_and();
  r.p.c = r.p.n;
}

auto R6502::opill_arr_immediate() {
L rd = readPCi();
  r.a &= rd;
  r.a = (r.p.c << 7) | (r.a >> 1);
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
  r.p.c = (r.a & 0x40);
  r.p.v = r.p.c ^ ((r.a >> 5) & 1);
}

auto R6502::opill_axs_immediate() {
L rd = readPCi();
  int result = (r.a & r.x) - rd;
  r.p.n = (result & 0x80);
  r.p.z = ((uint8)result == 0);
  r.p.c = (result >= 0);
  r.x = result;
}

auto R6502::opill_dcp_absolute_y() {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.y);
  rd = read(abs.w + r.y);
  write(abs.w + r.y, rd);
  op_dec();
  op_cmp();
L write(abs.w + r.y, rd);
}

auto R6502::opill_dcp_indirect_zero_page_x() {
  zp = readPCi();
  readZP(zp);
  abs.l = readZP(zp++ + r.x);
  abs.h = readZP(zp++ + r.x);
  rd = read(abs.w);
  write(abs.w, rd);
  op_dec();
  op_cmp();
L write(abs.w, rd);
}

auto R6502::opill_dcp_indirect_zero_page_y() {
  rd = readPCi();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  pageAlways(abs.w, abs.w + r.y);
  rd = read(abs.w + r.y);
  write(abs.w + r.y, rd);
  op_dec();
  op_cmp();
L write(abs.w + r.y, rd);
}

auto R6502::opill_las_absolute_y() {
  abs.l = readPCi();
  abs.h = readPCi();
  page(abs.w, abs.w + r.y);
L rd = read(abs.w + r.y);
  r.a = r.x = r.s = rd & r.s;
}

auto R6502::opill_lxa_immediate() {
  // line noise on the data bus interferes with what would be LAX #i.
L rd = readPCi();
  r.a = r.x = (r.a | (rand() & 0xff)) & rd;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::opill_nop_absolute() {
  abs.l = readPCi();
  abs.h = readPCi();
L readPC();
}

auto R6502::opill_nop_absolute_x() {
  abs.l = readPCi();
  abs.h = readPCi();
  page(abs.w, abs.w + r.x);
L readPC();
}

auto R6502::opill_nop_immediate() {
L rd = readPCi();
}

auto R6502::opill_nop_implied() {
L readPC();
}

auto R6502::opill_nop_zero_page() {
  zp = readPCi();
L readZP(zp);
}

auto R6502::opill_nop_zero_page_x() {
  zp = readPCi();
  readZP(zp);
L readZP(zp + r.x);
}

auto R6502::opill_sha_absolute_y() {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, r.a & r.x & (abs.h + 1));
}

auto R6502::opill_sha_indirect_zero_page_y() {
  rd = readPCi();
  abs.l = readZP(rd++);
  abs.h = readZP(rd++);
  pageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, r.a & r.x & (abs.h + 1));
}

auto R6502::opill_shx_absolute_y() {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.y);
L write(abs.w + r.y, r.x & (abs.h + 1));
}

auto R6502::opill_shy_absolute_x() {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.x);
L write(abs.w + r.x, r.y & (abs.h + 1));
}

auto R6502::opill_stp() {
  while(true) readPC();
}

auto R6502::opill_tas_absolute_y() {
  abs.l = readPCi();
  abs.h = readPCi();
  pageAlways(abs.w, abs.w + r.y);
  r.s = r.a & r.x;
L write(abs.w + r.y, r.a & r.x & (abs.h + 1));
}

auto R6502::opill_xaa_immediate() {
  rd = readPCi();
  r.a = (r.a | (rand() & 0xff)) & r.x & rd;
}
