auto R6502::opill_nop_immediate() {
L rd = readPCi();
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

auto R6502::opill_nop_zero_page() {
  zp = readPCi();
L readZP(zp);
}

auto R6502::opill_nop_zero_page_x() {
  zp = readPCi();
  readZP(zp);
L readZP(zp + r.x);
}

auto R6502::opill_stp() {
  while(true) readPC();
}

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
L rd = readPCi();
  //While other LAX opcodes decode to LDA and LDX, this one decodes
  //to LDA, LDX, and TAX, causing line noise on the data bus to interfere.
  //http://atariage.com/forums/topic/168616-lxa-stable/
  r.a = r.x = (r.a | (rand() & 0xff)) & rd;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
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
