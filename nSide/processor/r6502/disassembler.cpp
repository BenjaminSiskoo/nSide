auto R6502::disassemble() -> string {
  return disassemble(r.pc);
}

auto R6502::disassemble(uint16 addr) -> string {
  string output = {hex(addr, 4L), "  "};

  auto abs = [&]() -> string { return {"$", hex(disassemblerRead(addr + 2), 2L), hex(disassemblerRead(addr + 1), 2L)}; };
  auto abx = [&]() -> string { return {"$", hex(disassemblerRead(addr + 2), 2L), hex(disassemblerRead(addr + 1), 2L), ",x"}; };
  auto aby = [&]() -> string { return {"$", hex(disassemblerRead(addr + 2), 2L), hex(disassemblerRead(addr + 1), 2L), ",y"}; };
  auto iab = [&]() -> string { return {"($", hex(disassemblerRead(addr + 2), 2L), hex(disassemblerRead(addr + 1), 2L), ")"}; };
  auto imm = [&]() -> string { return {"#$", hex(disassemblerRead(addr + 1), 2L)}; };
  auto imp = [&]() -> string { return {""}; };
  auto izx = [&]() -> string { return {"($", hex(disassemblerRead(addr + 1), 2L), ",x)"}; };
  auto izy = [&]() -> string { return {"($", hex(disassemblerRead(addr + 1), 2L), "),y"}; };
  auto rel = [&]() -> string { return {"$", hex((addr + 2) + (int8)disassemblerRead(addr + 1), 4L)}; };
  auto zpg = [&]() -> string { return {"$", hex(disassemblerRead(addr + 1), 2L)}; };
  auto zpx = [&]() -> string { return {"$", hex(disassemblerRead(addr + 1), 2L), ",x"}; };
  auto zpy = [&]() -> string { return {"$", hex(disassemblerRead(addr + 1), 2L), ",y"}; };

  #define op(byte, prefix, mode) \
    case byte: output.append(#prefix, " ", mode()); \
    break

  uint8 opcode = disassemblerRead(addr);
  switch(opcode) {
    op(0x00, brk, imm);
    op(0x01, ora, izx);
    op(0x02, stp, imp); // I
    op(0x03, slo, izx); // I
    op(0x04, nop, zpg); // I
    op(0x05, ora, zpg);
    op(0x06, asl, zpg);
    op(0x07, slo, zpg); // I
    op(0x08, php, imp);
    op(0x09, ora, imm);
    op(0x0a, asl, imp);
    op(0x0b, anc, imm); // I
    op(0x0c, nop, abs); // I
    op(0x0d, ora, abs);
    op(0x0e, asl, abs);
    op(0x0f, slo, abs); // I
    op(0x10, bpl, rel);
    op(0x11, ora, izy);
    op(0x12, stp, imp); // I
    op(0x13, slo, izy); // I
    op(0x14, nop, zpx); // I
    op(0x15, ora, zpx);
    op(0x16, asl, zpx);
    op(0x17, slo, zpx); // I
    op(0x18, clc, imp);
    op(0x19, ora, aby);
    op(0x1a, nop, imp); // I
    op(0x1b, slo, aby); // I
    op(0x1c, nop, abx); // I
    op(0x1d, ora, abx);
    op(0x1e, asl, abx);
    op(0x1f, slo, abx); // I
    op(0x20, jsr, abs);
    op(0x21, and, izx);
    op(0x22, stp, imp); // I
    op(0x23, rla, izx); // I
    op(0x24, bit, zpg);
    op(0x25, and, zpg);
    op(0x26, rol, zpg);
    op(0x27, rla, zpg); // I
    op(0x28, plp, imp);
    op(0x29, and, imm);
    op(0x2a, rol, imp);
    op(0x2b, anc, imm); // I
    op(0x2c, bit, abs);
    op(0x2d, and, abs);
    op(0x2e, rol, abs);
    op(0x2f, rla, abs); // I
    op(0x30, bmi, rel);
    op(0x31, and, izy);
    op(0x32, stp, imp); // I
    op(0x33, rla, izy); // I
    op(0x34, nop, zpx); // I
    op(0x35, and, zpx);
    op(0x36, rol, zpx);
    op(0x37, rla, zpx); // I
    op(0x38, sec, imp);
    op(0x39, and, aby);
    op(0x3a, nop, imp); // I
    op(0x3b, rla, aby); // I
    op(0x3c, nop, abx); // I
    op(0x3d, and, abx);
    op(0x3e, rol, abx);
    op(0x3f, rla, abx); // I
    op(0x40, rti, imp);
    op(0x41, eor, izx);
    op(0x42, stp, imp); // I
    op(0x43, sre, izx); // I
    op(0x44, nop, zpg); // I
    op(0x45, eor, zpg);
    op(0x46, lsr, zpg);
    op(0x47, sre, zpg); // I
    op(0x48, pha, imp);
    op(0x49, eor, imm);
    op(0x4a, lsr, imp);
    op(0x4b, alr, imm); // I
    op(0x4c, jmp, abs);
    op(0x4d, eor, abs);
    op(0x4e, lsr, abs);
    op(0x4f, sre, abs); // I
    op(0x50, bvc, rel);
    op(0x51, eor, izy);
    op(0x52, stp, imp); // I
    op(0x53, sre, izy); // I
    op(0x54, nop, zpx); // I
    op(0x55, eor, zpx);
    op(0x56, lsr, zpx);
    op(0x57, sre, zpx); // I
    op(0x58, cli, imp);
    op(0x59, eor, aby);
    op(0x5a, nop, imp); // I
    op(0x5b, sre, aby); // I
    op(0x5c, nop, abx); // I
    op(0x5d, eor, abx);
    op(0x5e, lsr, abx);
    op(0x5f, sre, abx); // I
    op(0x60, rts, imp);
    op(0x61, adc, izx);
    op(0x62, stp, imp); // I
    op(0x63, rra, izx); // I
    op(0x64, nop, zpg); // I
    op(0x65, adc, zpg);
    op(0x66, ror, zpg);
    op(0x67, rra, zpg); // I
    op(0x68, pla, imp);
    op(0x69, adc, imm);
    op(0x6a, ror, imp);
    op(0x6b, arr, imm); // I
    op(0x6c, jmp, iab);
    op(0x6d, adc, abs);
    op(0x6e, ror, abs);
    op(0x6f, rra, abs); // I
    op(0x70, bvs, rel);
    op(0x71, adc, izy);
    op(0x72, stp, imp); // I
    op(0x73, rra, izy); // I
    op(0x74, nop, zpx); // I
    op(0x75, adc, zpx);
    op(0x76, ror, zpx);
    op(0x77, rra, zpx); // I
    op(0x78, sei, imp);
    op(0x79, adc, aby);
    op(0x7a, nop, imp); // I
    op(0x7b, rra, aby); // I
    op(0x7c, nop, abx); // I
    op(0x7d, adc, abx);
    op(0x7e, ror, abx);
    op(0x7f, rra, abx); // I
    op(0x80, nop, imm); // I
    op(0x81, sta, izx);
    op(0x82, nop, imm); // I
    op(0x83, sax, izx); // I
    op(0x84, sty, zpg);
    op(0x85, sta, zpg);
    op(0x86, stx, zpg);
    op(0x87, sax, zpg); // I
    op(0x88, dey, imp);
    op(0x89, nop, imm); // I
    op(0x8a, txa, imp);
    op(0x8b, xaa, imm); // I
    op(0x8c, sty, abs);
    op(0x8d, sta, abs);
    op(0x8e, stx, abs);
    op(0x8f, sax, abs); // I
    op(0x90, bcc, rel);
    op(0x91, sta, izy);
    op(0x92, stp, imp); // I
    op(0x93, sha, izy); // I
    op(0x94, sty, zpx);
    op(0x95, sta, zpx);
    op(0x96, stx, zpy);
    op(0x97, sax, zpy); // I
    op(0x98, tya, imp);
    op(0x99, sta, aby);
    op(0x9a, txs, imp);
    op(0x9b, tas, aby); // I
    op(0x9c, shy, abx); // I
    op(0x9d, sta, abx);
    op(0x9e, shx, imm); // I
    op(0x9f, sha, imm); // I
    op(0xa0, ldy, imm);
    op(0xa1, lda, izx);
    op(0xa2, ldx, imm);
    op(0xa3, lax, izx); // I
    op(0xa4, ldy, zpg);
    op(0xa5, lda, zpg);
    op(0xa6, ldx, zpg);
    op(0xa7, lax, zpg); // I
    op(0xa8, tay, imp);
    op(0xa9, lda, imm);
    op(0xaa, tax, imp);
    op(0xab, lxa, imm); // I
    op(0xac, ldy, abs);
    op(0xad, lda, abs);
    op(0xae, ldx, abs);
    op(0xaf, lax, abs); // I
    op(0xb0, bcs, rel);
    op(0xb1, lda, izy);
    op(0xb2, stp, imp); // I
    op(0xb3, lax, izy); // I
    op(0xb4, ldy, zpx);
    op(0xb5, lda, zpx);
    op(0xb6, ldx, zpy);
    op(0xb7, lax, zpy); // I
    op(0xb8, clv, imp);
    op(0xb9, lda, aby);
    op(0xba, tsx, imp);
    op(0xbb, las, aby); // I
    op(0xbc, ldy, abx);
    op(0xbd, lda, abx);
    op(0xbe, ldx, aby);
    op(0xbf, lax, aby); // I
    op(0xc0, cpy, imm);
    op(0xc1, cmp, izx);
    op(0xc2, nop, imm); // I
    op(0xc3, dcp, izx); // I
    op(0xc4, cpy, zpg);
    op(0xc5, cmp, zpg);
    op(0xc6, dec, zpg);
    op(0xc7, dcp, zpg); // I
    op(0xc8, iny, imp);
    op(0xc9, cmp, imm);
    op(0xca, dex, imp);
    op(0xcb, axs, imm); // I
    op(0xcc, cpy, abs);
    op(0xcd, cmp, abs);
    op(0xce, dec, abs);
    op(0xcf, dcp, abs); // I
    op(0xd0, bne, rel);
    op(0xd1, cmp, izy);
    op(0xd2, stp, imp); // I
    op(0xd3, dcp, izy); // I
    op(0xd4, nop, zpx); // I
    op(0xd5, cmp, zpx);
    op(0xd6, dec, zpx);
    op(0xd7, dcp, zpx); // I
    op(0xd8, cld, imp);
    op(0xd9, cmp, aby);
    op(0xda, nop, imp); // I
    op(0xdb, dcp, aby); // I
    op(0xdc, nop, abx); // I
    op(0xdd, cmp, abx);
    op(0xde, dec, abx);
    op(0xdf, dcp, abs); // I
    op(0xe0, cpx, imm);
    op(0xe1, sbc, izx);
    op(0xe2, nop, imm); // I
    op(0xe3, isc, zpx); // I
    op(0xe4, cpx, zpg);
    op(0xe5, sbc, zpg);
    op(0xe6, inc, zpg);
    op(0xe7, isc, zpg); // I
    op(0xe8, inx, imp);
    op(0xe9, sbc, imm);
    op(0xea, nop, imp);
    op(0xeb, sbc, imm); // I
    op(0xec, cpx, abs);
    op(0xed, sbc, abs);
    op(0xee, inc, abs);
    op(0xef, isc, abs); // I
    op(0xf0, beq, rel);
    op(0xf1, sbc, izy);
    op(0xf2, stp, imp); // I
    op(0xf3, isc, izy); // I
    op(0xf4, nop, zpx); // I
    op(0xf5, sbc, zpx);
    op(0xf6, inc, zpx);
    op(0xf7, isc, zpx); // I
    op(0xf8, sed, imp);
    op(0xf9, sbc, aby);
    op(0xfa, nop, imp);
    op(0xfb, isc, aby);
    op(0xfc, nop, abx); // I
    op(0xfd, sbc, abx);
    op(0xfe, inc, abx);
    op(0xff, isc, abx); // I

    default: output.append("$", hex(opcode, 2L)); break;
  }

  #undef op

  output.append("                ");
  output.resize(20);

  output.append(
    "A:", hex(r.a, 2L), " X:", hex(r.x, 2L), " Y:", hex(r.y, 2L), " S:", hex(r.s, 2L), " ",
    r.p.n ? "N" : "n", r.p.v ? "V" : "v", r.p.d ? "D" : "d",
    r.p.i ? "I" : "i", r.p.z ? "Z" : "z", r.p.c ? "C" : "c"
  );

  return output;
}
