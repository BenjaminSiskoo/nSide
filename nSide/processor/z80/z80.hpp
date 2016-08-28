#pragma once

//Zilog Z80

namespace Processor {

struct Z80 {
  virtual auto wait() -> void = 0;
  virtual auto read(uint16 addr) -> uint8 = 0;
  virtual auto write(uint16 addr, uint8 data) -> void = 0;
  virtual auto in(uint8 addr) -> uint8 = 0;
  virtual auto out(uint8 addr, uint8 data) -> void = 0;
  virtual auto stop() -> bool = 0;
  virtual auto debuggerRead(uint16 addr) -> uint8 { return 0; }

  //z80.cpp
  auto power() -> void;
  auto reset() -> void;

  auto parity(uint8_t) const -> bool;

  auto interrupt(uint16 vector) -> void;

  //instruction.cpp
  auto trap(uint8_t prefix, uint8_t opcode) -> void;
  auto instruction() -> void;
  auto instructionDD() -> void;
  auto instructionED() -> void;
  auto instructionFD() -> void;

  //legacy-lr35902-switch.cpp
  auto legacyInstruction(uint8 opcode) -> void;
  auto legacyInstructionCB(uint8 opcode) -> void;
  auto legacyInstructionED(uint8 opcode) -> void;
  auto legacyInstructionIndex(uint8 opcode, uint16_t&) -> void;
  auto legacyInstructionIndexCB(uint8 opcode, uint16_t&) -> void;

  //instructions.cpp
  auto CP(uint8 x) -> void;
  auto instructionCP_ihl() -> void;
  auto instructionCP_n() -> void;
  auto instructionCP_r(uint8_t&) -> void;
  auto instructionDI() -> void;
  auto instructionIM(uint) -> void;
  auto instructionIN_a_in() -> void;
  auto instructionIN_r_ic(uint8_t&) -> void;
  auto instructionJP_c_nn(bool) -> void;
  auto instructionJP_rr(uint16_t&) -> void;
  auto instructionJR_c(bool) -> void;
  auto instructionNOP() -> void;

  //disassembler.cpp
  auto disassemble(uint16 pc) -> string;
  auto disassembleOpcode(uint16 pc) -> string;
  auto disassembleOpcodeDD(uint16 pc) -> string;
  auto disassembleOpcodeED(uint16 pc) -> string;
  auto disassembleOpcodeFD(uint16 pc) -> string;

  struct Registers {
    union {
      uint16_t af;
      struct { uint8_t order_msb2(a, f); };
      union {
        BooleanBitField<uint16_t, 0> c;  //carry
        BooleanBitField<uint16_t, 1> n;  //add / subtract
        BooleanBitField<uint16_t, 2> p;  //parity
        BooleanBitField<uint16_t, 2> v;  //overflow
        BooleanBitField<uint16_t, 3> x;  //unused (copy of bit 3 of result)
        BooleanBitField<uint16_t, 4> h;  //half-carry
        BooleanBitField<uint16_t, 5> y;  //unused (copy of bit 5 of result)
        BooleanBitField<uint16_t, 6> z;  //zero
        BooleanBitField<uint16_t, 7> s;  //sign
      } p;
    };

    union {
      uint16_t bc;
      struct { uint8_t order_msb2(b, c); };
    };

    union {
      uint16_t de;
      struct { uint8_t order_msb2(d, e); };
    };

    union {
      uint16_t hl;
      struct { uint8_t order_msb2(h, l); };
    };

    uint16_t ix;
    uint16_t iy;
    uint16_t sp;
    uint16_t pc;

    uint8_t i;
    uint8_t r;

    boolean di;  //disable interrupt
    boolean ei;  //enable interrupt
    uint2 im;    //interrupt mode (0-2)

    boolean halt;
    boolean stop;
    boolean ime;

    uint8_t flag;
  } r;

  auto serialize(serializer&) -> void;

privileged:
  //8-bit load commands
  auto op_ld_r_r(uint8_t& x, uint8_t& y) -> void;
  auto op_ld_r_n(uint8_t& x) -> void;
  auto op_ld_r_hl(uint8_t& x) -> void;
  auto op_ld_hl_r(uint8_t& x) -> void;
  auto op_ld_hl_n() -> void;
  auto op_ld_a_rr(uint16_t& x) -> void;
  auto op_ld_rr_a(uint16_t& x) -> void;
  auto op_ldi_hl_a() -> void;
  auto op_ldi_a_hl() -> void;
  auto op_ldd_hl_a() -> void;
  auto op_ldd_a_hl() -> void;

  //16-bit load commands
  auto op_ld_rr_nn(uint16_t& x) -> void;
  auto op_ld_nn_sp() -> void;
  auto op_ld_sp_rr(uint16_t& x) -> void;
  auto op_push_rr(uint16_t& x) -> void;
  auto op_pop_rr(uint16_t& x) -> void;

  //8-bit arithmetic commands
  auto opi_add_a(uint8 x) -> void;
  auto op_add_a_r(uint8_t& x) -> void;
  auto op_add_a_n() -> void;
  auto op_add_a_hl() -> void;

  auto opi_adc_a(uint8 x) -> void;
  auto op_adc_a_r(uint8_t& x) -> void;
  auto op_adc_a_n() -> void;
  auto op_adc_a_hl() -> void;

  auto opi_sub_a(uint8 x) -> void;
  auto op_sub_a_r(uint8_t& x) -> void;
  auto op_sub_a_n() -> void;
  auto op_sub_a_hl() -> void;

  auto opi_sbc_a(uint8 x) -> void;
  auto op_sbc_a_r(uint8_t& x) -> void;
  auto op_sbc_a_n() -> void;
  auto op_sbc_a_hl() -> void;

  auto opi_and_a(uint8 x) -> void;
  auto op_and_a_r(uint8_t& x) -> void;
  auto op_and_a_n() -> void;
  auto op_and_a_hl() -> void;

  auto opi_xor_a(uint8 x) -> void;
  auto op_xor_a_r(uint8_t& x) -> void;
  auto op_xor_a_n() -> void;
  auto op_xor_a_hl() -> void;

  auto opi_or_a(uint8 x) -> void;
  auto op_or_a_r(uint8_t& x) -> void;
  auto op_or_a_n() -> void;
  auto op_or_a_hl() -> void;

  auto op_inc_r(uint8_t& x) -> void;
  auto op_inc_hl() -> void;
  auto op_dec_r(uint8_t& x) -> void;
  auto op_dec_hl() -> void;
  auto op_daa() -> void;
  auto op_cpl() -> void;

  //16-bit arithmetic commands
  auto op_add_hl_rr(uint16_t& x) -> void;
  auto op_inc_rr(uint16_t& x) -> void;
  auto op_dec_rr(uint16_t& x) -> void;

  //rotate/shift commands
  auto op_rlca() -> void;
  auto op_rla() -> void;
  auto op_rrca() -> void;
  auto op_rra() -> void;
  auto op_rlc_r(uint8_t& x) -> void;
  auto op_rlc_hl() -> void;
  auto op_rl_r(uint8_t& x) -> void;
  auto op_rl_hl() -> void;
  auto op_rrc_r(uint8_t& x) -> void;
  auto op_rrc_hl() -> void;
  auto op_rr_r(uint8_t& x) -> void;
  auto op_rr_hl() -> void;
  auto op_sla_r(uint8_t& x) -> void;
  auto op_sla_hl() -> void;
  auto op_swap_r(uint8_t& x) -> void;
  auto op_swap_hl() -> void;
  auto op_sra_r(uint8_t& x) -> void;
  auto op_sra_hl() -> void;
  auto op_srl_r(uint8_t& x) -> void;
  auto op_srl_hl() -> void;

  //single-bit commands
  auto op_bit_n_r(uint b, uint8_t& x) -> void;
  auto op_bit_n_hl(uint b) -> void;
  auto op_set_n_r(uint b, uint8_t& x) -> void;
  auto op_set_n_hl(uint b) -> void;
  auto op_res_n_r(uint b, uint8_t& x) -> void;
  auto op_res_n_hl(uint b) -> void;

  //control commands
  auto op_ccf() -> void;
  auto op_scf() -> void;
  auto op_halt() -> void;
  auto op_stop() -> void;
  auto op_ei() -> void;

  //jump commands
  auto op_call_nn() -> void;
  auto op_call_f_nn(bool x, bool y) -> void;
  auto op_ret() -> void;
  auto op_ret_f(bool x, bool y) -> void;
  auto op_reti() -> void;
  auto op_rst_n(uint n) -> void;

  //port commands
  auto op_out_n_a(uint8_t x) -> void;
  auto op_out_c_r(uint8_t x) -> void;

private:
  uint64 instructionsExecuted = 0;
};

}
