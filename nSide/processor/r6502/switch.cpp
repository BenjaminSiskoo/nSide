auto R6502::instruction() -> void {
  #define I

  switch(readPCi()) {
  case 0x00: return op_brk();
  case 0x01: return op_read_indirect_zero_page_x(&R6502::op_ora);
I case 0x02: return opill_stp();
I case 0x03: return opill_rmwr_indirect_zero_page_x(&R6502::op_asl, &R6502::op_ora);
I case 0x04: return opill_nop_zero_page();
  case 0x05: return op_read_zero_page(&R6502::op_ora);
  case 0x06: return op_rmw_zero_page(&R6502::op_asl);
I case 0x07: return opill_rmwr_zero_page(&R6502::op_asl, &R6502::op_ora);
  case 0x08: return op_php();
  case 0x09: return op_read_immediate(&R6502::op_ora);
  case 0x0a: return op_shift(&R6502::op_sla);
I case 0x0b: return opill_anc_immediate();
I case 0x0c: return opill_nop_absolute();
  case 0x0d: return op_read_absolute(&R6502::op_ora);
  case 0x0e: return op_rmw_absolute(&R6502::op_asl);
I case 0x0f: return opill_rmwr_absolute(&R6502::op_asl, &R6502::op_ora);
  case 0x10: return op_branch(r.p.n, 0);
  case 0x11: return op_read_indirect_zero_page_y(&R6502::op_ora);
I case 0x12: return opill_stp();
I case 0x13: return opill_rmwr_indirect_zero_page_y(&R6502::op_asl, &R6502::op_ora);
I case 0x14: return opill_nop_zero_page_x();
  case 0x15: return op_read_zero_page_x(&R6502::op_ora);
  case 0x16: return op_rmw_zero_page_x(&R6502::op_asl);
I case 0x17: return opill_rmwr_zero_page_x(&R6502::op_asl, &R6502::op_ora);
  case 0x18: return op_clear_flag(r.p.c.bit);
  case 0x19: return op_read_absolute_y(&R6502::op_ora);
I case 0x1a: return op_nop();
I case 0x1b: return opill_rmwr_absolute_y(&R6502::op_asl, &R6502::op_ora);
I case 0x1c: return opill_nop_absolute_x();
  case 0x1d: return op_read_absolute_x(&R6502::op_ora);
  case 0x1e: return op_rmw_absolute_x(&R6502::op_asl);
I case 0x1f: return opill_rmwr_absolute_x(&R6502::op_asl, &R6502::op_ora);
  case 0x20: return op_jsr_absolute();
  case 0x21: return op_read_indirect_zero_page_x(&R6502::op_and);
I case 0x22: return opill_stp();
I case 0x23: return opill_rmwr_indirect_zero_page_x(&R6502::op_rol, &R6502::op_and);
  case 0x24: return op_read_zero_page(&R6502::op_bit);
  case 0x25: return op_read_zero_page(&R6502::op_and);
  case 0x26: return op_rmw_zero_page(&R6502::op_rol);
I case 0x27: return opill_rmwr_zero_page(&R6502::op_rol, &R6502::op_and);
  case 0x28: return op_plp();
  case 0x29: return op_read_immediate(&R6502::op_and);
  case 0x2a: return op_shift(&R6502::op_rla);
I case 0x2b: return opill_anc_immediate();
  case 0x2c: return op_read_absolute(&R6502::op_bit);
  case 0x2d: return op_read_absolute(&R6502::op_and);
  case 0x2e: return op_rmw_absolute(&R6502::op_rol);
I case 0x2f: return opill_rmwr_absolute(&R6502::op_rol, &R6502::op_and);
  case 0x30: return op_branch(r.p.n, 1);
  case 0x31: return op_read_indirect_zero_page_y(&R6502::op_and);
I case 0x32: return opill_stp();
I case 0x33: return opill_rmwr_indirect_zero_page_y(&R6502::op_rol, &R6502::op_and);
I case 0x34: return opill_nop_zero_page_x();
  case 0x35: return op_read_zero_page_x(&R6502::op_and);
  case 0x36: return op_rmw_zero_page_x(&R6502::op_rol);
I case 0x37: return opill_rmwr_zero_page_x(&R6502::op_rol, &R6502::op_and);
  case 0x38: return op_set_flag(r.p.c.bit);
  case 0x39: return op_read_absolute_y(&R6502::op_and);
I case 0x3a: return op_nop();
I case 0x3b: return opill_rmwr_absolute_y(&R6502::op_rol, &R6502::op_and);
I case 0x3c: return opill_nop_absolute_x();
  case 0x3d: return op_read_absolute_x(&R6502::op_and);
  case 0x3e: return op_rmw_absolute_x(&R6502::op_rol);
I case 0x3f: return opill_rmwr_absolute_x(&R6502::op_rol, &R6502::op_and);
  case 0x40: return op_rti();
  case 0x41: return op_read_indirect_zero_page_x(&R6502::op_eor);
I case 0x42: return opill_stp();
I case 0x43: return opill_rmwr_indirect_zero_page_x(&R6502::op_lsr, &R6502::op_eor);
I case 0x44: return opill_nop_zero_page();
  case 0x45: return op_read_zero_page(&R6502::op_eor);
  case 0x46: return op_rmw_zero_page(&R6502::op_lsr);
I case 0x47: return opill_rmwr_zero_page(&R6502::op_lsr, &R6502::op_eor);
  case 0x48: return op_push(r.a);
  case 0x49: return op_read_immediate(&R6502::op_eor);
  case 0x4a: return op_shift(&R6502::op_sra);
I case 0x4b: return opill_alr_immediate();
  case 0x4c: return op_jmp_absolute();
  case 0x4d: return op_read_absolute(&R6502::op_eor);
  case 0x4e: return op_rmw_absolute(&R6502::op_lsr);
I case 0x4f: return opill_rmwr_absolute(&R6502::op_lsr, &R6502::op_eor);
  case 0x50: return op_branch(r.p.v, 0);
  case 0x51: return op_read_indirect_zero_page_y(&R6502::op_eor);
I case 0x52: return opill_stp();
I case 0x53: return opill_rmwr_indirect_zero_page_y(&R6502::op_lsr, &R6502::op_eor);
I case 0x54: return opill_nop_zero_page_x();
  case 0x55: return op_read_zero_page_x(&R6502::op_eor);
  case 0x56: return op_rmw_zero_page_x(&R6502::op_lsr);
I case 0x57: return opill_rmwr_zero_page_x(&R6502::op_lsr, &R6502::op_eor);
  case 0x58: return op_clear_flag(r.p.i.bit);
  case 0x59: return op_read_absolute_y(&R6502::op_eor);
I case 0x5a: return op_nop();
I case 0x5b: return opill_rmwr_absolute_y(&R6502::op_lsr, &R6502::op_eor);
I case 0x5c: return opill_nop_absolute_x();
  case 0x5d: return op_read_absolute_x(&R6502::op_eor);
  case 0x5e: return op_rmw_absolute_x(&R6502::op_lsr);
I case 0x5f: return opill_rmwr_absolute_x(&R6502::op_lsr, &R6502::op_eor);
  case 0x60: return op_rts();
  case 0x61: return op_read_indirect_zero_page_x(&R6502::op_adc);
I case 0x62: return opill_stp();
I case 0x63: return opill_rmwr_indirect_zero_page_x(&R6502::op_ror, &R6502::op_adc);
I case 0x64: return opill_nop_zero_page();
  case 0x65: return op_read_zero_page(&R6502::op_adc);
  case 0x66: return op_rmw_zero_page(&R6502::op_ror);
I case 0x67: return opill_rmwr_zero_page(&R6502::op_ror, &R6502::op_adc);
  case 0x68: return op_pull(r.a);
  case 0x69: return op_read_immediate(&R6502::op_adc);
  case 0x6a: return op_shift(&R6502::op_rra);
I case 0x6b: return opill_arr_immediate();
  case 0x6c: return op_jmp_indirect_absolute();
  case 0x6d: return op_read_absolute(&R6502::op_adc);
  case 0x6e: return op_rmw_absolute(&R6502::op_ror);
I case 0x6f: return opill_rmwr_absolute(&R6502::op_ror, &R6502::op_adc);
  case 0x70: return op_branch(r.p.v, 1);
  case 0x71: return op_read_indirect_zero_page_y(&R6502::op_adc);
I case 0x72: return opill_stp();
I case 0x73: return opill_rmwr_indirect_zero_page_y(&R6502::op_ror, &R6502::op_adc);
I case 0x74: return opill_nop_zero_page_x();
  case 0x75: return op_read_zero_page_x(&R6502::op_adc);
  case 0x76: return op_rmw_zero_page_x(&R6502::op_ror);
I case 0x77: return opill_rmwr_zero_page_x(&R6502::op_ror, &R6502::op_adc);
  case 0x78: return op_set_flag(r.p.i.bit);
  case 0x79: return op_read_absolute_y(&R6502::op_adc);
I case 0x7a: return op_nop();
I case 0x7b: return opill_rmwr_absolute_y(&R6502::op_ror, &R6502::op_adc);
I case 0x7c: return opill_nop_absolute_x();
  case 0x7d: return op_read_absolute_x(&R6502::op_adc);
  case 0x7e: return op_rmw_absolute_x(&R6502::op_ror);
I case 0x7f: return opill_rmwr_absolute_x(&R6502::op_ror, &R6502::op_adc);
I case 0x80: return opill_nop_immediate(); // r.y
  case 0x81: return op_store_indirect_zero_page_x(r.a);
I case 0x82: return opill_nop_immediate(); // r.x
I case 0x83: return op_store_indirect_zero_page_x(r.a & r.x);
  case 0x84: return op_store_zero_page(r.y);
  case 0x85: return op_store_zero_page(r.a);
  case 0x86: return op_store_zero_page(r.x);
I case 0x87: return op_store_zero_page(r.a & r.x);
  case 0x88: return op_decrement(r.y);
I case 0x89: return opill_nop_immediate(); // r.a
  case 0x8a: return op_transfer(r.x, r.a, 1);
I case 0x8b: return opill_xaa_immediate();
  case 0x8c: return op_store_absolute(r.y);
  case 0x8d: return op_store_absolute(r.a);
  case 0x8e: return op_store_absolute(r.x);
I case 0x8f: return op_store_absolute(r.a & r.x);
  case 0x90: return op_branch(r.p.c, 0);
  case 0x91: return op_store_indirect_zero_page_y(r.a);
I case 0x92: return opill_stp();
I case 0x93: return opill_sha_indirect_zero_page_y();
  case 0x94: return op_store_zero_page_x(r.y);
  case 0x95: return op_store_zero_page_x(r.a);
  case 0x96: return op_store_zero_page_y(r.x);
I case 0x97: return op_store_zero_page_y(r.a & r.x);
  case 0x98: return op_transfer(r.y, r.a, 1);
  case 0x99: return op_store_absolute_y(r.a);
  case 0x9a: return op_transfer(r.x, r.s, 0);
I case 0x9b: return opill_tas_absolute_y();
I case 0x9c: return opill_shy_absolute_x();
  case 0x9d: return op_store_absolute_x(r.a);
I case 0x9e: return opill_shx_absolute_y();
I case 0x9f: return opill_sha_absolute_y();
  case 0xa0: return op_read_immediate(&R6502::op_ldy);
  case 0xa1: return op_read_indirect_zero_page_x(&R6502::op_lda);
  case 0xa2: return op_read_immediate(&R6502::op_ldx);
I case 0xa3: return op_read_indirect_zero_page_x(&R6502::opill_lax);
  case 0xa4: return op_read_zero_page(&R6502::op_ldy);
  case 0xa5: return op_read_zero_page(&R6502::op_lda);
  case 0xa6: return op_read_zero_page(&R6502::op_ldx);
I case 0xa7: return op_read_zero_page(&R6502::opill_lax);
  case 0xa8: return op_transfer(r.a, r.y, 1);
  case 0xa9: return op_read_immediate(&R6502::op_lda);
  case 0xaa: return op_transfer(r.a, r.x, 1);
I case 0xab: return opill_lxa_immediate();
  case 0xac: return op_read_absolute(&R6502::op_ldy);
  case 0xad: return op_read_absolute(&R6502::op_lda);
  case 0xae: return op_read_absolute(&R6502::op_ldx);
I case 0xaf: return op_read_absolute(&R6502::opill_lax);
  case 0xb0: return op_branch(r.p.c, 1);
  case 0xb1: return op_read_indirect_zero_page_y(&R6502::op_lda);
I case 0xb2: return opill_stp();
I case 0xb3: return op_read_indirect_zero_page_y(&R6502::opill_lax);
  case 0xb4: return op_read_zero_page_x(&R6502::op_ldy);
  case 0xb5: return op_read_zero_page_x(&R6502::op_lda);
  case 0xb6: return op_read_zero_page_y(&R6502::op_ldx);
I case 0xb7: return op_read_zero_page_y(&R6502::opill_lax);
  case 0xb8: return op_clear_flag(r.p.v.bit);
  case 0xb9: return op_read_absolute_y(&R6502::op_lda);
  case 0xba: return op_transfer(r.s, r.x, 1);
I case 0xbb: return opill_las_absolute_y();
  case 0xbc: return op_read_absolute_x(&R6502::op_ldy);
  case 0xbd: return op_read_absolute_x(&R6502::op_lda);
  case 0xbe: return op_read_absolute_y(&R6502::op_ldx);
I case 0xbf: return op_read_absolute_y(&R6502::opill_lax);
  case 0xc0: return op_read_immediate(&R6502::op_cpy);
  case 0xc1: return op_read_indirect_zero_page_x(&R6502::op_cmp);
I case 0xc2: return opill_nop_immediate();
I case 0xc3: return opill_dcp_indirect_zero_page_x();
  case 0xc4: return op_read_zero_page(&R6502::op_cpy);
  case 0xc5: return op_read_zero_page(&R6502::op_cmp);
  case 0xc6: return op_rmw_zero_page(&R6502::op_dec);
I case 0xc7: return op_rmw_zero_page(&R6502::opill_dcp);
  case 0xc8: return op_increment(r.y);
  case 0xc9: return op_read_immediate(&R6502::op_cmp);
  case 0xca: return op_decrement(r.x);
I case 0xcb: return opill_axs_immediate();
  case 0xcc: return op_read_absolute(&R6502::op_cpy);
  case 0xcd: return op_read_absolute(&R6502::op_cmp);
  case 0xce: return op_rmw_absolute(&R6502::op_dec);
I case 0xcf: return op_rmw_absolute(&R6502::opill_dcp);
  case 0xd0: return op_branch(r.p.z, 0);
  case 0xd1: return op_read_indirect_zero_page_y(&R6502::op_cmp);
I case 0xd2: return opill_stp();
I case 0xd3: return opill_dcp_indirect_zero_page_y();
I case 0xd4: return opill_nop_zero_page_x();
  case 0xd5: return op_read_zero_page_x(&R6502::op_cmp);
  case 0xd6: return op_rmw_zero_page_x(&R6502::op_dec);
I case 0xd7: return op_rmw_zero_page_x(&R6502::opill_dcp);
  case 0xd8: return op_clear_flag(r.p.d.bit);
  case 0xd9: return op_read_absolute_y(&R6502::op_cmp);
I case 0xda: return op_nop();
I case 0xdb: return opill_dcp_absolute_y();
I case 0xdc: return opill_nop_absolute_x();
  case 0xdd: return op_read_absolute_x(&R6502::op_cmp);
  case 0xde: return op_rmw_absolute_x(&R6502::op_dec);
I case 0xdf: return op_rmw_absolute_x(&R6502::opill_dcp);
  case 0xe0: return op_read_immediate(&R6502::op_cpx);
  case 0xe1: return op_read_indirect_zero_page_x(&R6502::op_sbc);
I case 0xe2: return opill_nop_immediate();
I case 0xe3: return opill_rmwr_indirect_zero_page_x(&R6502::op_inc, &R6502::op_sbc);
  case 0xe4: return op_read_zero_page(&R6502::op_cpx);
  case 0xe5: return op_read_zero_page(&R6502::op_sbc);
  case 0xe6: return op_rmw_zero_page(&R6502::op_inc);
I case 0xe7: return opill_rmwr_zero_page(&R6502::op_inc, &R6502::op_sbc);
  case 0xe8: return op_increment(r.x);
  case 0xe9: return op_read_immediate(&R6502::op_sbc);
  case 0xea: return op_nop();
I case 0xeb: return op_read_immediate(&R6502::op_sbc);
  case 0xec: return op_read_absolute(&R6502::op_cpx);
  case 0xed: return op_read_absolute(&R6502::op_sbc);
  case 0xee: return op_rmw_absolute(&R6502::op_inc);
I case 0xef: return opill_rmwr_absolute(&R6502::op_inc, &R6502::op_sbc);
  case 0xf0: return op_branch(r.p.z, 1);
  case 0xf1: return op_read_indirect_zero_page_y(&R6502::op_sbc);
I case 0xf2: return opill_stp();
I case 0xf3: return opill_rmwr_indirect_zero_page_y(&R6502::op_inc, &R6502::op_sbc);
I case 0xf4: return opill_nop_zero_page_x();
  case 0xf5: return op_read_zero_page_x(&R6502::op_sbc);
  case 0xf6: return op_rmw_zero_page_x(&R6502::op_inc);
I case 0xf7: return opill_rmwr_zero_page_x(&R6502::op_inc, &R6502::op_sbc);
  case 0xf8: return op_set_flag(r.p.d.bit);
  case 0xf9: return op_read_absolute_y(&R6502::op_sbc);
I case 0xfa: return op_nop();
I case 0xfb: return opill_rmwr_absolute_y(&R6502::op_inc, &R6502::op_sbc);
I case 0xfc: return opill_nop_absolute_x();
  case 0xfd: return op_read_absolute_x(&R6502::op_sbc);
  case 0xfe: return op_rmw_absolute_x(&R6502::op_inc);
I case 0xff: return opill_rmwr_absolute_x(&R6502::op_inc, &R6502::op_sbc);
  }

  #undef I
}
