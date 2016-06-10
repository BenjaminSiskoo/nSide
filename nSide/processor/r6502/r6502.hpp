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

  virtual auto disassemblerRead(uint16 addr) -> uint8 { return 0u; }

  auto mdr() const -> uint8;
  auto power() -> void;
  auto reset() -> void;
  auto interrupt() -> void;
  auto instruction() -> void;

  //algorithms.cpp
  auto op_asl();
  auto op_adc();
  auto op_and();
  auto op_bit();
  auto op_cmp();
  auto op_cpx();
  auto op_cpy();
  auto op_dec();
  auto op_eor();
  auto op_inc();
  auto op_lda();
  auto op_ldx();
  auto op_ldy();
  auto op_lsr();
  auto op_ora();
  auto op_rla();
  auto op_rol();
  auto op_ror();
  auto op_rra();
  auto op_sbc();
  auto op_sla();
  auto op_sra();
  auto opill_dcp();
  auto opill_lax();

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
  auto opill_nop_immediate();
  auto opill_nop_absolute();
  auto opill_nop_absolute_x();
  auto opill_nop_zero_page();
  auto opill_nop_zero_page_x();
  auto opill_stp();
  auto opill_rmwr_absolute(fp, fp);
  auto opill_rmwr_absolute_x(fp, fp);
  auto opill_rmwr_absolute_y(fp, fp);
  auto opill_rmwr_indirect_zero_page_x(fp, fp);
  auto opill_rmwr_indirect_zero_page_y(fp, fp);
  auto opill_rmwr_zero_page(fp, fp);
  auto opill_rmwr_zero_page_x(fp, fp);
  auto opill_alr_immediate();
  auto opill_anc_immediate();
  auto opill_arr_immediate();
  auto opill_axs_immediate();
  auto opill_dcp_absolute_y();
  auto opill_dcp_indirect_zero_page_x();
  auto opill_dcp_indirect_zero_page_y();
  auto opill_las_absolute_y();
  auto opill_lxa_immediate();
  auto opill_sha_absolute_y();
  auto opill_sha_indirect_zero_page_y();
  auto opill_shx_absolute_y();
  auto opill_shy_absolute_x();
  auto opill_tas_absolute_y();
  auto opill_xaa_immediate();

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
