//opcode functions
//================

auto R6502::fp_adc() {
  int result = r.a + rd + r.p.c;
  r.p.v = ~(r.a ^ rd) & (r.a ^ result) & 0x80;
  r.p.c = (result > 0xff);
  r.p.n = (result & 0x80);
  r.p.z = ((uint8)result == 0);
  r.a = result;
}

auto R6502::fp_and() {
  r.a &= rd;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_asl() {
  r.p.c = rd & 0x80;
  rd <<= 1;
  r.p.n = (rd & 0x80);
  r.p.z = (rd == 0);
}

auto R6502::fp_bit() {
  r.p.n = (rd & 0x80);
  r.p.v = (rd & 0x40);
  r.p.z = ((rd & r.a) == 0);
}

auto R6502::fp_cmp() {
  int result = r.a - rd;
  r.p.n = (result & 0x80);
  r.p.z = (uint8)(result == 0);
  r.p.c = (result >= 0);
}

auto R6502::fp_cpx() {
  int result = r.x - rd;
  r.p.n = (result & 0x80);
  r.p.z = (uint8)(result == 0);
  r.p.c = (result >= 0);
}

auto R6502::fp_cpy() {
  int result = r.y - rd;
  r.p.n = (result & 0x80);
  r.p.z = (uint8)(result == 0);
  r.p.c = (result >= 0);
}

auto R6502::fp_dec() {
  rd--;
  r.p.n = (rd & 0x80);
  r.p.z = (rd == 0);
}

auto R6502::fp_eor() {
  r.a ^= rd;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_inc() {
  rd++;
  r.p.n = (rd & 0x80);
  r.p.z = (rd == 0);
}

auto R6502::fp_lda() {
  r.a = rd;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_ldx() {
  r.x = rd;
  r.p.n = (r.x & 0x80);
  r.p.z = (r.x == 0);
}

auto R6502::fp_ldy() {
  r.y = rd;
  r.p.n = (r.y & 0x80);
  r.p.z = (r.y == 0);
}

auto R6502::fp_lsr() {
  r.p.c = rd & 0x01;
  rd >>= 1;
  r.p.n = (rd & 0x80);
  r.p.z = (rd == 0);
}

auto R6502::fp_ora() {
  r.a |= rd;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_rla() {
  uint carry = (uint)r.p.c;
  r.p.c = r.a & 0x80;
  r.a = (r.a << 1) | carry;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_rol() {
  uint carry = (uint)r.p.c;
  r.p.c = rd & 0x80;
  rd = (rd << 1) | carry;
  r.p.n = (rd & 0x80);
  r.p.z = (rd == 0);
}

auto R6502::fp_ror() {
  uint carry = (uint)r.p.c << 7;
  r.p.c = rd & 0x01;
  rd = carry | (rd >> 1);
  r.p.n = (rd & 0x80);
  r.p.z = (rd == 0);
}

auto R6502::fp_rra() {
  uint carry = (uint)r.p.c << 7;
  r.p.c = r.a & 0x01;
  r.a = carry | (r.a >> 1);
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_sbc() {
  rd ^= 0xff;
  return fp_adc();
}

auto R6502::fp_sla() {
  r.p.c = r.a & 0x80;
  r.a <<= 1;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

auto R6502::fp_sra() {
  r.p.c = r.a & 0x01;
  r.a >>= 1;
  r.p.n = (r.a & 0x80);
  r.p.z = (r.a == 0);
}

//illegal opcode functions
//========================

auto R6502::fp_dcp() {
  fp_dec();
  fp_cmp();
}

auto R6502::fp_lax() {
  fp_lda();
  r.x = r.a;
}
