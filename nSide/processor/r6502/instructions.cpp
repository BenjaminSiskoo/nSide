//opcode functions
//================

auto R6502::opf_adc() {
  int result = regs.a + rd + regs.p.c;
  regs.p.v = ~(regs.a ^ rd) & (regs.a ^ result) & 0x80;
  regs.p.c = (result > 0xff);
  regs.p.n = (result & 0x80);
  regs.p.z = ((uint8)result == 0);
  regs.a = result;
}

auto R6502::opf_and() {
  regs.a &= rd;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_asl() {
  regs.p.c = rd & 0x80;
  rd <<= 1;
  regs.p.n = (rd & 0x80);
  regs.p.z = (rd == 0);
}

auto R6502::opf_bit() {
  regs.p.n = (rd & 0x80);
  regs.p.v = (rd & 0x40);
  regs.p.z = ((rd & regs.a) == 0);
}

auto R6502::opf_cmp() {
  int r = regs.a - rd;
  regs.p.n = (r & 0x80);
  regs.p.z = (uint8)(r == 0);
  regs.p.c = (r >= 0);
}

auto R6502::opf_cpx() {
  int r = regs.x - rd;
  regs.p.n = (r & 0x80);
  regs.p.z = (uint8)(r == 0);
  regs.p.c = (r >= 0);
}

auto R6502::opf_cpy() {
  int r = regs.y - rd;
  regs.p.n = (r & 0x80);
  regs.p.z = (uint8)(r == 0);
  regs.p.c = (r >= 0);
}

auto R6502::opf_dec() {
  rd--;
  regs.p.n = (rd & 0x80);
  regs.p.z = (rd == 0);
}

auto R6502::opf_eor() {
  regs.a ^= rd;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_inc() {
  rd++;
  regs.p.n = (rd & 0x80);
  regs.p.z = (rd == 0);
}

auto R6502::opf_lda() {
  regs.a = rd;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_ldx() {
  regs.x = rd;
  regs.p.n = (regs.x & 0x80);
  regs.p.z = (regs.x == 0);
}

auto R6502::opf_ldy() {
  regs.y = rd;
  regs.p.n = (regs.y & 0x80);
  regs.p.z = (regs.y == 0);
}

auto R6502::opf_lsr() {
  regs.p.c = rd & 0x01;
  rd >>= 1;
  regs.p.n = (rd & 0x80);
  regs.p.z = (rd == 0);
}

auto R6502::opf_ora() {
  regs.a |= rd;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_rla() {
  uint carry = (uint)regs.p.c;
  regs.p.c = regs.a & 0x80;
  regs.a = (regs.a << 1) | carry;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_rol() {
  uint carry = (uint)regs.p.c;
  regs.p.c = rd & 0x80;
  rd = (rd << 1) | carry;
  regs.p.n = (rd & 0x80);
  regs.p.z = (rd == 0);
}

auto R6502::opf_ror() {
  uint carry = (uint)regs.p.c << 7;
  regs.p.c = rd & 0x01;
  rd = carry | (rd >> 1);
  regs.p.n = (rd & 0x80);
  regs.p.z = (rd == 0);
}

auto R6502::opf_rra() {
  uint carry = (uint)regs.p.c << 7;
  regs.p.c = regs.a & 0x01;
  regs.a = carry | (regs.a >> 1);
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_sbc() {
  rd ^= 0xff;
  return opf_adc();
}

auto R6502::opf_sla() {
  regs.p.c = regs.a & 0x80;
  regs.a <<= 1;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opf_sra() {
  regs.p.c = regs.a & 0x01;
  regs.a >>= 1;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

//opcode implementations
//======================

auto R6502::opi_branch(bool condition) {
  if(condition == false) {
L   rd = op_readpci();
  } else {
    rd = op_readpci();
    aa = regs.pc + (int8)rd;
    op_page(regs.pc, aa);
L   op_readpc();
    regs.pc = aa;
  }
}

auto R6502::opi_clear_flag(bool& flag) {
L op_readpc();
  flag = 0;
}

auto R6502::opi_decrement(uint8& r) {
L op_readpc();
  r--;
  regs.p.n = (r & 0x80);
  regs.p.z = (r == 0);
}

auto R6502::opi_increment(uint8& r) {
L op_readpc();
  r++;
  regs.p.n = (r & 0x80);
  regs.p.z = (r == 0);
}

auto R6502::opi_pull(uint8& r) {
  op_readpc();
  op_readpc();
L r = op_readsp();
  regs.p.n = (r & 0x80);
  regs.p.z = (r == 0);
}

auto R6502::opi_push(uint8& r) {
  op_readpc();
L op_writesp(r);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_absolute() {
  abs.l = op_readpci();
  abs.h = op_readpci();
L rd = op_read(abs.w);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_absolute_x() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page(abs.w, abs.w + regs.x);
L rd = op_read(abs.w + regs.x);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page(abs.w, abs.w + regs.y);
L rd = op_read(abs.w + regs.y);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_immediate() {
L rd = op_readpci();
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_indirect_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
  abs.l = op_readzp(zp++ + regs.x);
  abs.h = op_readzp(zp++ + regs.x);
L rd = op_read(abs.w);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_indirect_zero_page_y() {
  rd = op_readpci();
  abs.l = op_readzp(rd++);
  abs.h = op_readzp(rd++);
  op_page(abs.w, abs.w + regs.y);
L rd = op_read(abs.w + regs.y);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_zero_page() {
  zp = op_readpci();
L rd = op_readzp(zp);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
L rd = op_readzp(zp + regs.x);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_read_zero_page_y() {
  zp = op_readpci();
  op_readzp(zp);
L rd = op_readzp(zp + regs.y);
  call(op);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_rmw_absolute() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  rd = op_read(abs.w);
  op_write(abs.w, rd);
  call(op);
L op_write(abs.w, rd);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_rmw_absolute_x() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.x);
  rd = op_read(abs.w + regs.x);
  op_write(abs.w + regs.x, rd);
  call(op);
L op_write(abs.w + regs.x, rd);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_rmw_zero_page() {
  zp = op_readpci();
  rd = op_readzp(zp);
  op_writezp(zp, rd);
  call(op);
L op_writezp(zp, rd);
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_rmw_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
  rd = op_readzp(zp + regs.x);
  op_writezp(zp + regs.x, rd);
  call(op);
L op_writezp(zp + regs.x, rd);
}

auto R6502::opi_set_flag(bool& flag) {
L op_readpc();
  flag = 1;
}

template<auto (R6502::*op)() -> void>
auto R6502::opi_shift() {
L op_readpc();
  call(op);
}

auto R6502::opi_store_absolute(uint8 r) {
  abs.l = op_readpci();
  abs.h = op_readpci();
L op_write(abs.w, r);
}

auto R6502::opi_store_absolute_x(uint8& r) {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.x);
L op_write(abs.w + regs.x, r);
}

auto R6502::opi_store_absolute_y(uint8& r) {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.y);
L op_write(abs.w + regs.y, r);
}

auto R6502::opi_store_indirect_zero_page_x(uint8 r) {
  zp = op_readpci();
  op_readzp(zp);
  abs.l = op_readzp(zp++ + regs.x);
  abs.h = op_readzp(zp++ + regs.x);
L op_write(abs.w, r);
}

auto R6502::opi_store_indirect_zero_page_y(uint8& r) {
  rd = op_readpci();
  abs.l = op_readzp(rd++);
  abs.h = op_readzp(rd++);
  op_page_always(abs.w, abs.w + regs.y);
L op_write(abs.w + regs.y, r);
}

auto R6502::opi_store_zero_page(uint8 r) {
  zp = op_readpci();
L op_writezp(zp, r);
}

auto R6502::opi_store_zero_page_x(uint8 r) {
  zp = op_readpci();
  op_readzp(zp);
L op_writezp(zp + regs.x, r);
}

auto R6502::opi_store_zero_page_y(uint8 r) {
  zp = op_readpci();
  op_readzp(zp);
L op_writezp(zp + regs.y, r);
}

auto R6502::opi_transfer(uint8& s, uint8& d, bool flag) {
L op_readpc();
  d = s;
  if(flag == false) return;
  regs.p.n = (d & 0x80);
  regs.p.z = (d == 0);
}

//opcodes
//=======

auto R6502::op_brk() {
  op_readpci();
  op_writesp(regs.pc >> 8);
  op_writesp(regs.pc >> 0);
  uint16 vector = 0xfffe;
  nmi(vector);
  op_writesp(regs.p | 0x30);
  abs.l = op_read(vector + 0);
  regs.p.i = 1;
L abs.h = op_read(vector + 1);
  regs.pc = abs.w;
}

auto R6502::op_jmp_absolute() {
  abs.l = op_readpci();
L abs.h = op_readpci();
  regs.pc = abs.w;
}

auto R6502::op_jmp_indirect_absolute() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  iabs.l = op_read(abs.w); abs.l++;
L iabs.h = op_read(abs.w); abs.l++;
  regs.pc = iabs.w;
}

auto R6502::op_jsr_absolute() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_readpc();
  regs.pc--;
  op_writesp(regs.pc >> 8);
L op_writesp(regs.pc >> 0);
  regs.pc = abs.w;
}

auto R6502::op_nop() {
L op_readpc();
}

auto R6502::op_php() {
  op_readpc();
L op_writesp(regs.p | 0x30);
}

auto R6502::op_plp() {
  op_readpc();
  op_readpc();
L regs.p = op_readsp();
}

auto R6502::op_rti() {
  op_readpc();
  op_readpc();
  regs.p = op_readsp();
  abs.l = op_readsp();
L abs.h = op_readsp();
  regs.pc = abs.w;
}

auto R6502::op_rts() {
  op_readpc();
  op_readpc();
  abs.l = op_readsp();
  abs.h = op_readsp();
L op_readpc();
  regs.pc = ++abs.w;
}

//illegal opcode functions
//========================

auto R6502::opillf_dcp() {
  opf_dec();
  opf_cmp();
}

auto R6502::opillf_lax() {
  opf_lda();
  regs.x = regs.a;
}

//illegal opcode implementations
//==============================

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_absolute() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  rd = op_read(abs.w);
  op_write(abs.w, rd);
  call(opw);
L op_write(abs.w, rd);
  call(opr);
}

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_absolute_x() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.x);
  rd = op_read(abs.w + regs.x);
  op_write(abs.w + regs.x, rd);
  call(opw);
L op_write(abs.w + regs.x, rd);
  call(opr);
}

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.y);
  rd = op_read(abs.w + regs.y);
  op_write(abs.w + regs.y, rd);
  call(opw);
L op_write(abs.w + regs.y, rd);
  call(opr);
}

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_indirect_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
  abs.l = op_readzp(zp++ + regs.x);
  abs.h = op_readzp(zp++ + regs.x);
  rd = op_read(abs.w);
  op_write(abs.w, rd);
  call(opw);
L op_write(abs.w, rd);
  call(opr);
}

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_indirect_zero_page_y() {
  rd = op_readpci();
  abs.l = op_readzp(rd++);
  abs.h = op_readzp(rd++);
  op_page_always(abs.w, abs.w + regs.y);
  rd = op_read(abs.w + regs.y);
  op_write(abs.w + regs.y, rd);
  call(opw);
L op_write(abs.w + regs.y, rd);
  call(opr);
}

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_zero_page() {
  zp = op_readpci();
  rd = op_readzp(zp);
  op_writezp(zp, rd);
  call(opw);
L op_writezp(zp, rd);
  call(opr);
}

template<auto (R6502::*opw)() -> void,auto (R6502::*opr)() -> void>
auto R6502::opilli_rmwr_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
  rd = op_readzp(zp + regs.x);
  op_writezp(zp + regs.x, rd);
  call(opw);
L op_writezp(zp + regs.x, rd);
  call(opr);
}

//illegal opcodes
//===============

auto R6502::opill_alr_immediate() {
  opi_read_immediate<&R6502::opf_and>();
  opf_sra();
}

auto R6502::opill_anc_immediate() {
L rd = op_readpci();
  opf_and();
  regs.p.c = regs.p.n;
}

auto R6502::opill_arr_immediate() {
L rd = op_readpci();
  regs.a &= rd;
  regs.a = (regs.p.c << 7) | (regs.a >> 1);
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
  regs.p.c = (regs.a & 0x40);
  regs.p.v = regs.p.c ^ ((regs.a >> 5) & 1);
}

auto R6502::opill_axs_immediate() {
L rd = op_readpci();
  int r = (regs.a & regs.x) - rd;
  regs.p.n = (r & 0x80);
  regs.p.z = ((uint8)r == 0);
  regs.p.c = (r >= 0);
  regs.x = r;
}

auto R6502::opill_dcp_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.y);
  rd = op_read(abs.w + regs.y);
  op_write(abs.w + regs.y, rd);
  opf_dec();
  opf_cmp();
L op_write(abs.w + regs.y, rd);
}

auto R6502::opill_dcp_indirect_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
  abs.l = op_readzp(zp++ + regs.x);
  abs.h = op_readzp(zp++ + regs.x);
  rd = op_read(abs.w);
  op_write(abs.w, rd);
  opf_dec();
  opf_cmp();
L op_write(abs.w, rd);
}

auto R6502::opill_dcp_indirect_zero_page_y() {
  rd = op_readpci();
  abs.l = op_readzp(rd++);
  abs.h = op_readzp(rd++);
  op_page_always(abs.w, abs.w + regs.y);
  rd = op_read(abs.w + regs.y);
  op_write(abs.w + regs.y, rd);
  opf_dec();
  opf_cmp();
L op_write(abs.w + regs.y, rd);
}

auto R6502::opill_las_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page(abs.w, abs.w + regs.y);
L rd = op_read(abs.w + regs.y);
  regs.a = regs.x = regs.s = rd & regs.s;
}

auto R6502::opill_lxa_immediate() {
  // line noise on the data bus interferes with what would be LAX #i.
L rd = op_readpci();
  regs.a = regs.x = (regs.a | (rand() & 0xff)) & rd;
  regs.p.n = (regs.a & 0x80);
  regs.p.z = (regs.a == 0);
}

auto R6502::opill_nop_absolute() {
  abs.l = op_readpci();
  abs.h = op_readpci();
L op_readpc();
}

auto R6502::opill_nop_absolute_x() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page(abs.w, abs.w + regs.x);
L op_readpc();
}

auto R6502::opill_nop_immediate() {
L rd = op_readpci();
}

auto R6502::opill_nop_implied() {
L op_readpc();
}

auto R6502::opill_nop_zero_page() {
  zp = op_readpci();
L op_readzp(zp);
}

auto R6502::opill_nop_zero_page_x() {
  zp = op_readpci();
  op_readzp(zp);
L op_readzp(zp + regs.x);
}

auto R6502::opill_sha_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.y);
L op_write(abs.w + regs.y, regs.a & regs.x & (abs.h + 1));
}

auto R6502::opill_sha_indirect_zero_page_y() {
  rd = op_readpci();
  abs.l = op_readzp(rd++);
  abs.h = op_readzp(rd++);
  op_page_always(abs.w, abs.w + regs.y);
L op_write(abs.w + regs.y, regs.a & regs.x & (abs.h + 1));
}

auto R6502::opill_shx_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.y);
L op_write(abs.w + regs.y, regs.x & (abs.h + 1));
}

auto R6502::opill_shy_absolute_x() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.x);
L op_write(abs.w + regs.x, regs.y & (abs.h + 1));
}

auto R6502::opill_stp() {
  while(true) op_readpc();
}

auto R6502::opill_tas_absolute_y() {
  abs.l = op_readpci();
  abs.h = op_readpci();
  op_page_always(abs.w, abs.w + regs.y);
  regs.s = regs.a & regs.x;
L op_write(abs.w + regs.y, regs.a & regs.x & (abs.h + 1));
}

auto R6502::opill_xaa_immediate() {
  rd = op_readpci();
  regs.a = (regs.a | (rand() & 0xff)) & regs.x & rd;
}
