//Ricoh 6502
//* Ricoh 2A03
//* Ricoh 2A07

#pragma once

namespace Processor {

struct R6502 {
  #include "registers.hpp"
  #include "memory.hpp"

  using fp = auto (R6502::*)() -> void;

  virtual auto read(uint16 addr) -> uint8 = 0;
  virtual auto write(uint16 addr, uint8 data) -> void = 0;
  virtual auto lastCycle() -> void = 0;
  virtual auto nmi(uint16& vector) -> void = 0;

  virtual auto readDisassembler(uint16 addr) -> uint8 { return 0; }

  //r6502.cpp
  auto mdr() const -> uint8;
  auto power() -> void;
  auto reset() -> void;
  auto interrupt() -> void;

  //algorithms.cpp
  auto fp_asl();
  auto fp_adc();
  auto fp_and();
  auto fp_bit();
  auto fp_cmp();
  auto fp_cpx();
  auto fp_cpy();
  auto fp_dec();
  auto fp_eor();
  auto fp_inc();
  auto fp_lda();
  auto fp_ldx();
  auto fp_ldy();
  auto fp_lsr();
  auto fp_ora();
  auto fp_rla();
  auto fp_rol();
  auto fp_ror();
  auto fp_rra();
  auto fp_sbc();
  auto fp_sla();
  auto fp_sra();
  auto fp_dcp();
  auto fp_lax();

  //opcode_read.cpp
  auto op_read_immediate(fp);
  auto op_read_absolute(fp);
  auto op_read_absolute_x(fp);
  auto op_read_absolute_y(fp);
  auto op_read_zero_page(fp);
  auto op_read_zero_page_x(fp);
  auto op_read_zero_page_y(fp);
  auto op_read_indirect_zero_page_x(fp);
  auto op_read_indirect_zero_page_y(fp);

  //opcode_write.cpp
  auto op_store_absolute(uint8);
  auto op_store_absolute_x(uint8&);
  auto op_store_absolute_y(uint8&);
  auto op_store_zero_page(uint8);
  auto op_store_zero_page_x(uint8);
  auto op_store_zero_page_y(uint8);
  auto op_store_indirect_zero_page_x(uint8);
  auto op_store_indirect_zero_page_y(uint8&);

  //opcode_rmw.cpp
  auto op_increment(uint8& r);
  auto op_decrement(uint8& r);
  auto op_shift(fp);
  auto op_rmw_absolute(fp);
  auto op_rmw_absolute_x(fp);
  auto op_rmw_zero_page(fp);
  auto op_rmw_zero_page_x(fp);

  //opcode_pc.cpp
  auto op_branch(bool flag, bool value);
  auto op_jmp_absolute();
  auto op_jmp_indirect_absolute();
  auto op_jsr_absolute();
  auto op_rti();
  auto op_rts();

  //opcode_misc.cpp
  auto op_nop();
  auto op_brk();
  auto op_set_flag(uint bit);
  auto op_clear_flag(uint bit);
  auto op_transfer(uint8& s, uint8& d, bool flag);
  auto op_push(uint8& r);
  auto op_php();
  auto op_pull(uint8& r);
  auto op_plp();

  //opcode_illegal.cpp
  auto op_nop_immediate();
  auto op_nop_absolute();
  auto op_nop_absolute_x();
  auto op_nop_zero_page();
  auto op_nop_zero_page_x();
  auto op_stp();
  auto op_rmwr_absolute(fp, fp);
  auto op_rmwr_absolute_x(fp, fp);
  auto op_rmwr_absolute_y(fp, fp);
  auto op_rmwr_indirect_zero_page_x(fp, fp);
  auto op_rmwr_indirect_zero_page_y(fp, fp);
  auto op_rmwr_zero_page(fp, fp);
  auto op_rmwr_zero_page_x(fp, fp);
  auto op_alr_immediate();
  auto op_anc_immediate();
  auto op_arr_immediate();
  auto op_axs_immediate();
  auto op_dcp_absolute_y();
  auto op_dcp_indirect_zero_page_x();
  auto op_dcp_indirect_zero_page_y();
  auto op_las_absolute_y();
  auto op_lxa_immediate();
  auto op_sha_absolute_y();
  auto op_sha_indirect_zero_page_y();
  auto op_shx_absolute_y();
  auto op_shy_absolute_x();
  auto op_tas_absolute_y();
  auto op_xaa_immediate();

  //switch.cpp
  auto instruction() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassemble() -> string;
  auto disassemble(uint16 addr) -> string;

  Registers r;
  Reg16 abs, iabs;
  uint8 rd, zp;
  uint16 aa;
};

}
