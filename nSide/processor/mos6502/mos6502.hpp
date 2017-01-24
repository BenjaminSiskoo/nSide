//MOS 6502
//* MOS 6507
//* Ricoh 2A03
//* Ricoh 2A07

#pragma once

namespace Processor {

struct MOS6502 {
  #include "registers.hpp"
  #include "memory.hpp"

  using fp = auto (MOS6502::*)() -> void;

  MOS6502(bool allowBCD);

  virtual auto read(uint16 addr) -> uint8 = 0;
  virtual auto write(uint16 addr, uint8 data) -> void = 0;
  virtual auto lastCycle() -> void = 0;
  virtual auto nmi(uint16& vector) -> void = 0;

  virtual auto readDisassembler(uint16 addr) -> uint8 { return 0; }

  //mos6502.cpp
  auto interrupt() -> void;
  auto mdr() const -> uint8;
  auto power() -> void;
  auto reset() -> void;

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

  //instruction.cpp
  auto instruction() -> void;

  //instructions-read.cpp
  auto op_read_immediate(fp) -> void;
  auto op_read_absolute(fp) -> void;
  auto op_read_absolute_x(fp) -> void;
  auto op_read_absolute_y(fp) -> void;
  auto op_read_zero_page(fp) -> void;
  auto op_read_zero_page_x(fp) -> void;
  auto op_read_zero_page_y(fp) -> void;
  auto op_read_indirect_zero_page_x(fp) -> void;
  auto op_read_indirect_zero_page_y(fp) -> void;

  //instructions-write.cpp
  auto op_store_absolute(uint8) -> void;
  auto op_store_absolute_x(uint8&) -> void;
  auto op_store_absolute_y(uint8&) -> void;
  auto op_store_zero_page(uint8) -> void;
  auto op_store_zero_page_x(uint8) -> void;
  auto op_store_zero_page_y(uint8) -> void;
  auto op_store_indirect_zero_page_x(uint8) -> void;
  auto op_store_indirect_zero_page_y(uint8&) -> void;

  //instructions-rmw.cpp
  auto op_increment(uint8& r) -> void;
  auto op_decrement(uint8& r) -> void;
  auto op_shift(fp) -> void;
  auto op_rmw_absolute(fp) -> void;
  auto op_rmw_absolute_x(fp) -> void;
  auto op_rmw_zero_page(fp) -> void;
  auto op_rmw_zero_page_x(fp) -> void;

  //instructions-pc.cpp
  auto op_branch(bool flag, bool value) -> void;
  auto op_jmp_absolute() -> void;
  auto op_jmp_indirect_absolute() -> void;
  auto op_jsr_absolute() -> void;
  auto op_rti() -> void;
  auto op_rts() -> void;

  //instructions-misc.cpp
  auto op_nop() -> void;
  auto op_brk() -> void;
  auto op_set_flag(uint bit) -> void;
  auto op_clear_flag(uint bit) -> void;
  auto op_transfer(uint8& s, uint8& d, bool flag) -> void;
  auto op_push(uint8& r) -> void;
  auto op_php() -> void;
  auto op_pull(uint8& r) -> void;
  auto op_plp() -> void;

  //instructions-illegal.cpp
  auto op_nop_immediate() -> void;
  auto op_nop_absolute() -> void;
  auto op_nop_absolute_x() -> void;
  auto op_nop_zero_page() -> void;
  auto op_nop_zero_page_x() -> void;
  auto op_stp() -> void;
  auto op_rmwr_absolute(fp, fp) -> void;
  auto op_rmwr_absolute_x(fp, fp) -> void;
  auto op_rmwr_absolute_y(fp, fp) -> void;
  auto op_rmwr_indirect_zero_page_x(fp, fp) -> void;
  auto op_rmwr_indirect_zero_page_y(fp, fp) -> void;
  auto op_rmwr_zero_page(fp, fp) -> void;
  auto op_rmwr_zero_page_x(fp, fp) -> void;
  auto op_alr_immediate() -> void;
  auto op_anc_immediate() -> void;
  auto op_arr_immediate() -> void;
  auto op_axs_immediate() -> void;
  auto op_dcp_absolute_y() -> void;
  auto op_dcp_indirect_zero_page_x() -> void;
  auto op_dcp_indirect_zero_page_y() -> void;
  auto op_las_absolute_y() -> void;
  auto op_lxa_immediate() -> void;
  auto op_sha_absolute_y() -> void;
  auto op_sha_indirect_zero_page_y() -> void;
  auto op_shx_absolute_y() -> void;
  auto op_shy_absolute_x() -> void;
  auto op_tas_absolute_y() -> void;
  auto op_xaa_immediate() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassemble() -> string;
  auto disassemble(uint16 addr) -> string;

  Registers r;
  Reg16 abs, iabs;
  uint8 rd, zp;
  uint16 aa;

  const bool allowBCD;
};

}
