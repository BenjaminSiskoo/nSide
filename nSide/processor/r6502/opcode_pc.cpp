auto R6502::op_branch(bool flag, bool value) {
  if(flag != value) {
L   rd = readPCi();
  } else {
    rd = readPCi();
    aa = r.pc + (int8)rd;
    page(r.pc, aa);
L   readPC();
    r.pc = aa;
  }
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
  r.pc.w--;
  writeSP(r.pc >> 8);
L writeSP(r.pc >> 0);
  r.pc.w = abs.w;
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
