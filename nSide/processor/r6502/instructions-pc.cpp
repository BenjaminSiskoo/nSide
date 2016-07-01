auto R6502::op_branch(bool flag, bool value) {
  if(flag != value) {
L   rd = readPC();
  } else {
    rd = readPC();
    aa = r.pc + (int8)rd;
    ioPage(r.pc, aa);
L   idle();
    r.pc = aa;
  }
}

auto R6502::op_jmp_absolute() {
  abs.l = readPC();
L abs.h = readPC();
  r.pc = abs.w;
}

auto R6502::op_jmp_indirect_absolute() {
  abs.l = readPC();
  abs.h = readPC();
  iabs.l = read(abs.w); abs.l++;
L iabs.h = read(abs.w); abs.l++;
  r.pc = iabs.w;
}

auto R6502::op_jsr_absolute() {
  abs.l = readPC();
  abs.h = readPC();
  idle();
  r.pc.w--;
  writeSP(r.pc >> 8);
L writeSP(r.pc >> 0);
  r.pc.w = abs.w;
}

auto R6502::op_rti() {
  idle();
  idle();
  r.p = readSP();
  abs.l = readSP();
L abs.h = readSP();
  r.pc = abs.w;
}

auto R6502::op_rts() {
  idle();
  idle();
  abs.l = readSP();
  abs.h = readSP();
L idle();
  r.pc = ++abs.w;
}
