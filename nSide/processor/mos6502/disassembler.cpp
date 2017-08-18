auto MOS6502::disassemble(uint16 pc) -> string {
  string s{hex(pc, 4L), "  "};

  auto absolute = [&]() -> string {
    return {"$", hex(readDebugger(pc + 2), 2L), hex(readDebugger(pc + 1), 2L)};
  };

  auto absoluteX = [&]() -> string {
    return {"$", hex(readDebugger(pc + 2), 2L), hex(readDebugger(pc + 1), 2L), ",x"};
  };

  auto absoluteY = [&]() -> string {
    return {"$", hex(readDebugger(pc + 2), 2L), hex(readDebugger(pc + 1), 2L), ",y"};
  };

  auto branch = [&]() -> string {
    return {"$", hex((pc + 2) + (int8)readDebugger(pc + 1), 4L)};
  };

  auto immediate = [&]() -> string {
    return {"#$", hex(readDebugger(pc + 1), 2L)};
  };

  auto implied = [&]() -> string {
    return {""};
  };

  auto indirect = [&]() -> string {
    return {"($", hex(readDebugger(pc + 2), 2L), hex(readDebugger(pc + 1), 2L), ")"};
  };

  auto indirectX = [&]() -> string {
    return {"($", hex(readDebugger(pc + 1), 2L), ",x)"};
  };

  auto indirectY = [&]() -> string {
    return {"($", hex(readDebugger(pc + 1), 2L), "),y"};
  };

  auto zeroPage = [&]() -> string {
    return {"$", hex(readDebugger(pc + 1), 2L)};
  };

  auto zeroPageX = [&]() -> string {
    return {"$", hex(readDebugger(pc + 1), 2L), ",x"};
  };

  auto zeroPageY = [&]() -> string {
    return {"$", hex(readDebugger(pc + 1), 2L), ",y"};
  };

  #define op(id, prefix, mode) \
    case id: s.append(prefix, " ", mode()); \
    break;

  switch(auto opcode = readDebugger(pc)) {
  op(0x00, "brk", immediate)
  op(0x01, "ora", indirectX)
  op(0x05, "ora", zeroPage)
  op(0x06, "asl", zeroPage)
  op(0x08, "php", implied)
  op(0x09, "ora", immediate)
  op(0x0a, "asl", implied)
  op(0x0d, "ora", absolute)
  op(0x0e, "asl", absolute)
  op(0x10, "bpl", branch)
  op(0x11, "ora", indirectY)
  op(0x15, "ora", zeroPageX)
  op(0x16, "asl", zeroPageX)
  op(0x18, "clc", implied)
  op(0x19, "ora", absoluteY)
  op(0x1d, "ora", absoluteX)
  op(0x1e, "asl", absoluteX)
  op(0x20, "jsr", absolute)
  op(0x21, "and", indirectX)
  op(0x24, "bit", zeroPage)
  op(0x25, "and", zeroPage)
  op(0x26, "rol", zeroPage)
  op(0x28, "plp", implied)
  op(0x29, "and", immediate)
  op(0x2a, "rol", implied)
  op(0x2c, "bit", absolute)
  op(0x2d, "and", absolute)
  op(0x2e, "rol", absolute)
  op(0x30, "bmi", branch)
  op(0x31, "and", indirectY)
  op(0x35, "and", zeroPageX)
  op(0x36, "rol", zeroPageX)
  op(0x38, "sec", implied)
  op(0x39, "and", absoluteY)
  op(0x3d, "and", absoluteX)
  op(0x3e, "rol", absoluteX)
  op(0x40, "rti", implied)
  op(0x41, "eor", indirectX)
  op(0x45, "eor", zeroPage)
  op(0x46, "lsr", zeroPage)
  op(0x48, "pha", implied)
  op(0x49, "eor", immediate)
  op(0x4a, "lsr", implied)
  op(0x4c, "jmp", absolute)
  op(0x4d, "eor", absolute)
  op(0x4e, "lsr", absolute)
  op(0x50, "bvc", branch)
  op(0x51, "eor", indirectY)
  op(0x55, "eor", zeroPageX)
  op(0x56, "lsr", zeroPageX)
  op(0x58, "cli", implied)
  op(0x59, "eor", absoluteY)
  op(0x5d, "eor", absoluteX)
  op(0x5e, "lsr", absoluteX)
  op(0x60, "rts", implied)
  op(0x61, "adc", indirectX)
  op(0x65, "adc", zeroPage)
  op(0x66, "ror", zeroPage)
  op(0x68, "pla", implied)
  op(0x69, "adc", immediate)
  op(0x6a, "ror", implied)
  op(0x6c, "jmp", indirect)
  op(0x6d, "adc", absolute)
  op(0x6e, "ror", absolute)
  op(0x70, "bvs", branch)
  op(0x71, "adc", indirectY)
  op(0x75, "adc", zeroPageX)
  op(0x76, "ror", zeroPageX)
  op(0x78, "sei", implied)
  op(0x79, "adc", absoluteY)
  op(0x7d, "adc", absoluteX)
  op(0x7e, "ror", absoluteX)
  op(0x81, "sta", indirectX)
  op(0x84, "sty", zeroPage)
  op(0x85, "sta", zeroPage)
  op(0x86, "stx", zeroPage)
  op(0x88, "dey", implied)
  op(0x8a, "txa", implied)
  op(0x8c, "sty", absolute)
  op(0x8d, "sta", absolute)
  op(0x8e, "stx", absolute)
  op(0x90, "bcc", branch)
  op(0x91, "sta", indirectY)
  op(0x94, "sty", zeroPageX)
  op(0x95, "sta", zeroPageX)
  op(0x96, "stx", zeroPageY)
  op(0x98, "tya", implied)
  op(0x99, "sta", absoluteY)
  op(0x9a, "txs", implied)
  op(0x9d, "sta", absoluteX)
  op(0xa0, "ldy", immediate)
  op(0xa1, "lda", indirectX)
  op(0xa2, "ldx", immediate)
  op(0xa4, "ldy", zeroPage)
  op(0xa5, "lda", zeroPage)
  op(0xa6, "ldx", zeroPage)
  op(0xa8, "tay", implied)
  op(0xa9, "lda", immediate)
  op(0xaa, "tax", implied)
  op(0xac, "ldy", absolute)
  op(0xad, "lda", absolute)
  op(0xae, "ldx", absolute)
  op(0xb0, "bcs", branch)
  op(0xb1, "lda", indirectY)
  op(0xb4, "ldy", zeroPageX)
  op(0xb5, "lda", zeroPageX)
  op(0xb6, "ldx", zeroPageY)
  op(0xb8, "clv", implied)
  op(0xb9, "lda", absoluteY)
  op(0xba, "tsx", implied)
  op(0xbc, "ldy", absoluteX)
  op(0xbd, "lda", absoluteX)
  op(0xbe, "ldx", absoluteY)
  op(0xc0, "cpy", immediate)
  op(0xc1, "cmp", indirectX)
  op(0xc4, "cpy", zeroPage)
  op(0xc5, "cmp", zeroPage)
  op(0xc6, "dec", zeroPage)
  op(0xc8, "iny", implied)
  op(0xc9, "cmp", immediate)
  op(0xca, "dex", implied)
  op(0xcc, "cpy", absolute)
  op(0xcd, "cmp", absolute)
  op(0xce, "dec", absolute)
  op(0xd0, "bne", branch)
  op(0xd1, "cmp", indirectY)
  op(0xd5, "cmp", zeroPageX)
  op(0xd6, "dec", zeroPageX)
  op(0xd8, "cld", implied)
  op(0xd9, "cmp", absoluteY)
  op(0xdd, "cmp", absoluteX)
  op(0xde, "dec", absoluteX)
  op(0xe0, "cpx", immediate)
  op(0xe1, "sbc", indirectX)
  op(0xe4, "cpx", zeroPage)
  op(0xe5, "sbc", zeroPage)
  op(0xe6, "inc", zeroPage)
  op(0xe8, "inx", implied)
  op(0xe9, "sbc", immediate)
  op(0xea, "nop", implied)
  op(0xec, "cpx", absolute)
  op(0xed, "sbc", absolute)
  op(0xee, "inc", absolute)
  op(0xf0, "beq", branch)
  op(0xf1, "sbc", indirectY)
  op(0xf5, "sbc", zeroPageX)
  op(0xf6, "inc", zeroPageX)
  op(0xf8, "sed", implied)
  op(0xf9, "sbc", absoluteY)
  op(0xfd, "sbc", absoluteX)
  op(0xfe, "inc", absoluteX)

  //unofficial instructions
  op(0x02, "stp", implied)
  op(0x03, "slo", indirectX)
  op(0x04, "nop", zeroPage)
  op(0x07, "slo", zeroPage)
  op(0x0b, "anc", immediate)
  op(0x0c, "nop", absolute)
  op(0x0f, "slo", absolute)
  op(0x12, "stp", implied)
  op(0x13, "slo", indirectY)
  op(0x14, "nop", zeroPageX)
  op(0x17, "slo", zeroPageX)
  op(0x1a, "nop", implied)
  op(0x1b, "slo", absoluteY)
  op(0x1c, "nop", absoluteX)
  op(0x1f, "slo", absoluteX)
  op(0x22, "stp", implied)
  op(0x23, "rla", indirectX)
  op(0x27, "rla", zeroPage)
  op(0x2b, "anc", immediate)
  op(0x2f, "rla", absolute)
  op(0x32, "stp", implied)
  op(0x33, "rla", indirectY)
  op(0x34, "nop", zeroPageX)
  op(0x37, "rla", zeroPageX)
  op(0x3a, "nop", implied)
  op(0x3b, "rla", absoluteY)
  op(0x3c, "nop", absoluteX)
  op(0x3f, "rla", absoluteX)
  op(0x42, "stp", implied)
  op(0x43, "sre", indirectX)
  op(0x44, "nop", zeroPage)
  op(0x47, "sre", zeroPage)
  op(0x4b, "alr", immediate)
  op(0x4f, "sre", absolute)
  op(0x52, "stp", implied)
  op(0x53, "sre", indirectY)
  op(0x54, "nop", zeroPageX)
  op(0x57, "sre", zeroPageX)
  op(0x5a, "nop", implied)
  op(0x5b, "sre", absoluteY)
  op(0x5c, "nop", absoluteX)
  op(0x5f, "sre", absoluteX)
  op(0x62, "stp", implied)
  op(0x63, "rra", indirectX)
  op(0x64, "nop", zeroPage)
  op(0x67, "rra", zeroPage)
  op(0x6b, "arr", immediate)
  op(0x6f, "rra", absolute)
  op(0x72, "stp", implied)
  op(0x73, "rra", indirectY)
  op(0x74, "nop", zeroPageX)
  op(0x77, "rra", zeroPageX)
  op(0x7a, "nop", implied)
  op(0x7b, "rra", absoluteY)
  op(0x7c, "nop", absoluteX)
  op(0x7f, "rra", absoluteX)
  op(0x80, "nop", immediate)
  op(0x82, "nop", immediate)
  op(0x83, "sax", indirectX)
  op(0x87, "sax", zeroPage)
  op(0x89, "nop", immediate)
  op(0x8b, "xaa", immediate)
  op(0x8f, "sax", absolute)
  op(0x92, "stp", implied)
  op(0x93, "ahx", indirectY)
  op(0x97, "sax", zeroPageY)
  op(0x9b, "tas", absoluteY)
  op(0x9c, "shy", absoluteX)
  op(0x9e, "shx", absoluteY)
  op(0x9f, "ahx", absoluteY)
  op(0xa3, "lax", indirectX)
  op(0xa7, "lax", zeroPage)
  op(0xab, "lxa", immediate)
  op(0xaf, "lax", absolute)
  op(0xb2, "stp", implied)
  op(0xb3, "lax", indirectY)
  op(0xb7, "lax", zeroPageY)
  op(0xbb, "las", absoluteY)
  op(0xbf, "lax", absoluteY)
  op(0xc2, "nop", immediate)
  op(0xc3, "dcp", indirectX)
  op(0xc7, "dcp", zeroPage)
  op(0xcb, "axs", immediate)
  op(0xcf, "dcp", absolute)
  op(0xd2, "stp", implied)
  op(0xd3, "dcp", indirectY)
  op(0xd4, "nop", zeroPageX)
  op(0xd7, "dcp", zeroPageX)
  op(0xda, "nop", implied)
  op(0xdb, "dcp", absoluteY)
  op(0xdc, "nop", absoluteX)
  op(0xdf, "dcp", absolute)
  op(0xe2, "nop", immediate)
  op(0xe3, "isc", zeroPageX)
  op(0xe7, "isc", zeroPage)
  op(0xeb, "sbc", immediate)
  op(0xef, "isc", absolute)
  op(0xf2, "stp", implied)
  op(0xf3, "isc", indirectY)
  op(0xf4, "nop", zeroPageX)
  op(0xf7, "isc", zeroPageX)
  op(0xfa, "nop", implied)
  op(0xfb, "isc", absoluteY)
  op(0xfc, "nop", absoluteX)
  op(0xff, "isc", absoluteX)

  default:
    s.append("$", hex(opcode, 2L));
    break;
  }

  #undef op

  s.append("                    ");
  s.resize(20);

  s.append(" A:", hex(A, 2L));
  s.append(" X:", hex(X, 2L));
  s.append(" Y:", hex(Y, 2L));
  s.append(" S:", hex(S, 2L));
  s.append(" ");
  s.append(N ? "N" : "n");
  s.append(V ? "V" : "v");
  s.append("-");
  s.append("-");
  s.append(D ? "D" : "d");
  s.append(I ? "I" : "i");
  s.append(Z ? "Z" : "z");
  s.append(C ? "C" : "c");

  return s;
}
