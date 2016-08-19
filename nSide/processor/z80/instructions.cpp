auto Z80::instructionDI() -> void {
}

auto Z80::instructionNOP() -> void {
}

auto Z80::op_cb() -> void {
  instructionCB();
}

auto Z80::op_dd() -> void {
  instructionIndex(r.ix);
}

auto Z80::op_ddcb() -> void {
  instructionIndexCB(r.ix);
}

auto Z80::op_ed() -> void {
  instructionED();
}

auto Z80::op_fd() -> void {
  instructionIndex(r.iy);
}

auto Z80::op_fdcb() -> void {
  instructionIndexCB(r.iy);
}

//8-bit load commands

auto Z80::op_ld_r_r(uint8_t& x, uint8_t& y) -> void {
  x = y;
}

auto Z80::op_ld_r_n(uint8_t& x) -> void {
  x = read(r.pc++);
}

auto Z80::op_ld_r_hl(uint8_t& x) -> void {
  x = read(r.hl);
}

auto Z80::op_ld_hl_r(uint8_t& x) -> void {
  write(r.hl, x);
}

auto Z80::op_ld_hl_n() -> void {
  write(r.hl, read(r.pc++));
}

auto Z80::op_ld_a_rr(uint16_t& x) -> void {
  r.a = read(x);
}

auto Z80::op_ld_a_nn() -> void {
  uint8 lo = read(r.pc++);
  uint8 hi = read(r.pc++);
  r.a = read((hi << 8) | (lo << 0));
}

auto Z80::op_ld_rr_a(uint16_t& x) -> void {
  write(x, r.a);
}

auto Z80::op_ld_nn_a() -> void {
  uint8 lo = read(r.pc++);
  uint8 hi = read(r.pc++);
  write((hi << 8) | (lo << 0), r.a);
}

auto Z80::op_ld_a_ffn() -> void {
  r.a = read(0xff00 + read(r.pc++));
}

auto Z80::op_ld_ffn_a() -> void {
  write(0xff00 + read(r.pc++), r.a);
}

auto Z80::op_ld_a_ffc() -> void {
  r.a = read(0xff00 + r.c);
}

auto Z80::op_ld_ffc_a() -> void {
  write(0xff00 + r.c, r.a);
}

auto Z80::op_ldi_hl_a() -> void {
  write(r.hl, r.a);
  r.hl++;
}

auto Z80::op_ldi_a_hl() -> void {
  r.a = read(r.hl);
  r.hl++;
}

auto Z80::op_ldd_hl_a() -> void {
  write(r.hl, r.a);
  r.hl--;
}

auto Z80::op_ldd_a_hl() -> void {
  r.a = read(r.hl);
  r.hl--;
}

//16-bit load commands

auto Z80::op_ld_rr_nn(uint16_t& x) -> void {
  x  = read(r.pc++) << 0;
  x |= read(r.pc++) << 8;
}

auto Z80::op_ld_nn_sp() -> void {
  uint16 addr = read(r.pc++) << 0;
  addr |= read(r.pc++) << 8;
  write(addr + 0, r.sp >> 0);
  write(addr + 1, r.sp >> 8);
}

auto Z80::op_ld_sp_hl() -> void {
  r.sp = r.hl;
  wait();
}

auto Z80::op_push_rr(uint16_t& x) -> void {
  wait();
  write(--r.sp, x >> 8);
  write(--r.sp, x >> 0);
}

auto Z80::op_pop_rr(uint16_t& x) -> void {
  x  = read(r.sp++) << 0;
  x |= read(r.sp++) << 8;
}

//8-bit arithmetic commands

auto Z80::opi_add_a(uint8 x) -> void {
  uint16 rh = r.a + x;
  uint16 rl = (r.a & 0x0f) + (x & 0x0f);
  r.a = rh;
  r.p.z = (uint8)rh == 0;
  r.p.n = 0;
  r.p.h = rl > 0x0f;
  r.p.c = rh > 0xff;
}

auto Z80::op_add_a_r(uint8_t& x) -> void { opi_add_a(x); }
auto Z80::op_add_a_n() -> void { opi_add_a(read(r.pc++)); }
auto Z80::op_add_a_hl() -> void { opi_add_a(read(r.hl)); }

auto Z80::opi_adc_a(uint8 x) -> void {
  uint16 rh = r.a + x + r.p.c;
  uint16 rl = (r.a & 0x0f) + (x & 0x0f) + r.p.c;
  r.a = rh;
  r.p.z = (uint8)rh == 0;
  r.p.n = 0;
  r.p.h = rl > 0x0f;
  r.p.c = rh > 0xff;
}

auto Z80::op_adc_a_r(uint8_t& x) -> void { opi_adc_a(x); }
auto Z80::op_adc_a_n() -> void { opi_adc_a(read(r.pc++)); }
auto Z80::op_adc_a_hl() -> void { opi_adc_a(read(r.hl)); }

auto Z80::opi_sub_a(uint8 x) -> void {
  uint16 rh = r.a - x;
  uint16 rl = (r.a & 0x0f) - (x & 0x0f);
  r.a = rh;
  r.p.z = (uint8)rh == 0;
  r.p.n = 1;
  r.p.h = rl > 0x0f;
  r.p.c = rh > 0xff;
}

auto Z80::op_sub_a_r(uint8_t& x) -> void { opi_sub_a(x); }
auto Z80::op_sub_a_n() -> void { opi_sub_a(read(r.pc++)); }
auto Z80::op_sub_a_hl() -> void { opi_sub_a(read(r.hl)); }

auto Z80::opi_sbc_a(uint8 x) -> void {
  uint16 rh = r.a - x - r.p.c;
  uint16 rl = (r.a & 0x0f) - (x & 0x0f) - r.p.c;
  r.a = rh;
  r.p.z = (uint8)rh == 0;
  r.p.n = 1;
  r.p.h = rl > 0x0f;
  r.p.c = rh > 0xff;
}

auto Z80::op_sbc_a_r(uint8_t& x) -> void { opi_sbc_a(x); }
auto Z80::op_sbc_a_n() -> void { opi_sbc_a(read(r.pc++)); }
auto Z80::op_sbc_a_hl() -> void { opi_sbc_a(read(r.hl)); }

auto Z80::opi_and_a(uint8 x) -> void {
  r.a &= x;
  r.p.z = r.a == 0;
  r.p.n = 0;
  r.p.h = 1;
  r.p.c = 0;
}

auto Z80::op_and_a_r(uint8_t& x) -> void { opi_and_a(x); }
auto Z80::op_and_a_n() -> void { opi_and_a(read(r.pc++)); }
auto Z80::op_and_a_hl() -> void { opi_and_a(read(r.hl)); }

auto Z80::opi_xor_a(uint8 x) -> void {
  r.a ^= x;
  r.p.z = r.a == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = 0;
}

auto Z80::op_xor_a_r(uint8_t& x) -> void { opi_xor_a(x); }
auto Z80::op_xor_a_n() -> void { opi_xor_a(read(r.pc++)); }
auto Z80::op_xor_a_hl() -> void { opi_xor_a(read(r.hl)); }

auto Z80::opi_or_a(uint8 x) -> void {
  r.a |= x;
  r.p.z = r.a == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = 0;
}

auto Z80::op_or_a_r(uint8_t& x) -> void { opi_or_a(x); }
auto Z80::op_or_a_n() -> void { opi_or_a(read(r.pc++)); }
auto Z80::op_or_a_hl() -> void { opi_or_a(read(r.hl)); }

auto Z80::opi_cp_a(uint8 x) -> void {
  uint16 rh = r.a - x;
  uint16 rl = (r.a & 0x0f) - (x & 0x0f);
  r.p.z = (uint8)rh == 0;
  r.p.n = 1;
  r.p.h = rl > 0x0f;
  r.p.c = rh > 0xff;
}

auto Z80::op_cp_a_r(uint8_t& x) -> void { opi_cp_a(x); }
auto Z80::op_cp_a_n() -> void { opi_cp_a(read(r.pc++)); }
auto Z80::op_cp_a_hl() -> void { opi_cp_a(read(r.hl)); }

auto Z80::op_inc_r(uint8_t& x) -> void {
  x++;
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = (x & 0x0f) == 0x00;
}

auto Z80::op_inc_hl() -> void {
  uint8 n = read(r.hl);
  write(r.hl, ++n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = (n & 0x0f) == 0x00;
}

auto Z80::op_dec_r(uint8_t& x) -> void {
  x--;
  r.p.z = x == 0;
  r.p.n = 1;
  r.p.h = (x & 0x0f) == 0x0f;
}

auto Z80::op_dec_hl() -> void {
  uint8 n = read(r.hl);
  write(r.hl, --n);
  r.p.z = n == 0;
  r.p.n = 1;
  r.p.h = (n & 0x0f) == 0x0f;
}

auto Z80::op_daa() -> void {
  uint16 a = r.a;
  if(r.p.n == 0) {
    if(r.p.h || (a & 0x0f) > 0x09) a += 0x06;
    if(r.p.c || (a       ) > 0x9f) a += 0x60;
  } else {
    if(r.p.h) {
      a -= 0x06;
      if(r.p.c == 0) a &= 0xff;
    }
    if(r.p.c) a -= 0x60;
  }
  r.a = a;
  r.p.z = r.a == 0;
  r.p.h = 0;
  r.p.c |= a & 0x100;
}

auto Z80::op_cpl() -> void {
  r.a ^= 0xff;
  r.p.n = 1;
  r.p.h = 1;
}

//16-bit arithmetic commands

auto Z80::op_add_hl_rr(uint16_t& x) -> void {
  wait();
  uint32 rb = (r.hl + x);
  uint32 rn = (r.hl & 0xfff) + (x & 0xfff);
  r.hl = rb;
  r.p.n = 0;
  r.p.h = rn > 0x0fff;
  r.p.c = rb > 0xffff;
}

auto Z80::op_inc_rr(uint16_t& x) -> void {
  wait();
  x++;
}

auto Z80::op_dec_rr(uint16_t& x) -> void {
  wait();
  x--;
}

auto Z80::op_add_sp_n() -> void {
  int n = (int8)read(r.pc++);
  r.p.z = 0;
  r.p.n = 0;
  r.p.h = ((r.sp & 0x0f) + (n & 0x0f)) > 0x0f;
  r.p.c = ((r.sp & 0xff) + (n & 0xff)) > 0xff;
  r.sp += n;
  wait();
  wait();
}

auto Z80::op_ld_hl_sp_n() -> void {
  int n = (int8)read(r.pc++);
  r.p.z = 0;
  r.p.n = 0;
  r.p.h = ((r.sp & 0x0f) + (n & 0x0f)) > 0x0f;
  r.p.c = ((r.sp & 0xff) + (n & 0xff)) > 0xff;
  r.hl = r.sp + n;
  wait();
}

//rotate/shift commands

auto Z80::op_rlca() -> void {
  r.a = (r.a << 1) | (r.a >> 7);
  r.p.z = 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = r.a & 0x01;
}

auto Z80::op_rla() -> void {
  bool c = r.a & 0x80;
  r.a = (r.a << 1) | (r.p.c << 0);
  r.p.z = 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_rrca() -> void {
  r.a = (r.a >> 1) | (r.a << 7);
  r.p.z = 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = r.a & 0x80;
}

auto Z80::op_rra() -> void {
  bool c = r.a & 0x01;
  r.a = (r.a >> 1) | (r.p.c << 7);
  r.p.z = 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_rlc_r(uint8_t& x) -> void {
  x = (x << 1) | (x >> 7);
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = x & 0x01;
}

auto Z80::op_rlc_hl() -> void {
  uint8 n = read(r.hl);
  n = (n << 1) | (n >> 7);
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = n & 0x01;
}

auto Z80::op_rl_r(uint8_t& x) -> void {
  bool c = x & 0x80;
  x = (x << 1) | (r.p.c << 0);
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_rl_hl() -> void {
  uint8 n = read(r.hl);
  bool c = n & 0x80;
  n = (n << 1) | (r.p.c << 0);
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_rrc_r(uint8_t& x) -> void {
  x = (x >> 1) | (x << 7);
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = x & 0x80;
}

auto Z80::op_rrc_hl() -> void {
  uint8 n = read(r.hl);
  n = (n >> 1) | (n << 7);
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = n & 0x80;
}

auto Z80::op_rr_r(uint8_t& x) -> void {
  bool c = x & 0x01;
  x = (x >> 1) | (r.p.c << 7);
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_rr_hl() -> void {
  uint8 n = read(r.hl);
  bool c = n & 0x01;
  n = (n >> 1) | (r.p.c << 7);
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_sla_r(uint8_t& x) -> void {
  bool c = x & 0x80;
  x <<= 1;
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_sla_hl() -> void {
  uint8 n = read(r.hl);
  bool c = n & 0x80;
  n <<= 1;
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_swap_r(uint8_t& x) -> void {
  x = (x << 4) | (x >> 4);
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = 0;
}

auto Z80::op_swap_hl() -> void {
  uint8 n = read(r.hl);
  n = (n << 4) | (n >> 4);
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = 0;
}

auto Z80::op_sra_r(uint8_t& x) -> void {
  bool c = x & 0x01;
  x = (int8)x >> 1;
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_sra_hl() -> void {
  uint8 n = read(r.hl);
  bool c = n & 0x01;
  n = (int8)n >> 1;
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_srl_r(uint8_t& x) -> void {
  bool c = x & 0x01;
  x >>= 1;
  r.p.z = x == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

auto Z80::op_srl_hl() -> void {
  uint8 n = read(r.hl);
  bool c = n & 0x01;
  n >>= 1;
  write(r.hl, n);
  r.p.z = n == 0;
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = c;
}

//single-bit commands

auto Z80::op_bit_n_r(uint b, uint8_t& x) -> void {
  r.p.z = (x & (1 << b)) == 0;
  r.p.n = 0;
  r.p.h = 1;
}

auto Z80::op_bit_n_hl(uint b) -> void {
  uint8 n = read(r.hl);
  r.p.z = (n & (1 << b)) == 0;
  r.p.n = 0;
  r.p.h = 1;
}

auto Z80::op_set_n_r(uint b, uint8_t& x) -> void {
  x |= 1 << b;
}

auto Z80::op_set_n_hl(uint b) -> void {
  uint8 n = read(r.hl);
  n |= 1 << b;
  write(r.hl, n);
}

auto Z80::op_res_n_r(uint b, uint8_t& x) -> void {
  x &= ~(1 << b);
}

auto Z80::op_res_n_hl(uint b) -> void {
  uint n = read(r.hl);
  n &= ~(1 << b);
  write(r.hl, n);
}

//control commands

auto Z80::op_ccf() -> void {
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = !r.p.c;
}

auto Z80::op_scf() -> void {
  r.p.n = 0;
  r.p.h = 0;
  r.p.c = 1;
}

auto Z80::op_halt() -> void {
  r.halt = true;
  while(r.halt == true) wait();
}

auto Z80::op_stop() -> void {
  if(stop()) return;
  r.stop = true;
  while(r.stop == true) wait();
}

auto Z80::op_ei() -> void {
  r.ei = true;
//r.ime = 1;
}

//jump commands

auto Z80::op_jp_nn() -> void {
  uint8 lo = read(r.pc++);
  uint8 hi = read(r.pc++);
  r.pc = (hi << 8) | (lo << 0);
  wait();
}

auto Z80::op_jp_hl() -> void {
  r.pc = r.hl;
}

auto Z80::op_jp_f_nn(bool x, bool y) -> void {
  uint8 lo = read(r.pc++);
  uint8 hi = read(r.pc++);
  if(x == y) {
    r.pc = (hi << 8) | (lo << 0);
    wait();
  }
}

auto Z80::op_jr_n() -> void {
  int8 n = read(r.pc++);
  r.pc += n;
  wait();
}

auto Z80::op_jr_f_n(bool x, bool y) -> void {
  int8 n = read(r.pc++);
  if(x == y) {
    r.pc += n;
    wait();
  }
}

auto Z80::op_call_nn() -> void {
  uint8 lo = read(r.pc++);
  uint8 hi = read(r.pc++);
  wait();
  write(--r.sp, r.pc >> 8);
  write(--r.sp, r.pc >> 0);
  r.pc = (hi << 8) | (lo << 0);
}

auto Z80::op_call_f_nn(bool x, bool y) -> void {
  uint8 lo = read(r.pc++);
  uint8 hi = read(r.pc++);
  if(x == y) {
    wait();
    write(--r.sp, r.pc >> 8);
    write(--r.sp, r.pc >> 0);
    r.pc = (hi << 8) | (lo << 0);
  }
}

auto Z80::op_ret() -> void {
  uint8 lo = read(r.sp++);
  uint8 hi = read(r.sp++);
  r.pc = (hi << 8) | (lo << 0);
  wait();
}

auto Z80::op_ret_f(bool x, bool y) -> void {
  wait();
  if(x == y) {
    uint8 lo = read(r.sp++);
    uint8 hi = read(r.sp++);
    r.pc = (hi << 8) | (lo << 0);
    wait();
  }
}

auto Z80::op_reti() -> void {
  uint8 lo = read(r.sp++);
  uint8 hi = read(r.sp++);
  r.pc = (hi << 8) | (lo << 0);
  wait();
  r.ime = 1;
}

auto Z80::op_rst_n(uint n) -> void {
  wait();
  write(--r.sp, r.pc >> 8);
  write(--r.sp, r.pc >> 0);
  r.pc = n;
}

auto Z80::op_out(uint8_t& x) -> void {
  uint8 port = read(r.pc++);
  wait();
  wait();
  portWrite(port, x);
}

auto Z80::op_in(uint8_t& x) -> void {
  uint8 port = read(r.pc++);
  wait();
  wait();
  x = portRead(port);
}
