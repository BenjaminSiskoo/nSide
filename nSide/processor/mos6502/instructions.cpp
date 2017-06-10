auto MOS6502::ADC(uint8 i) -> uint8 {
  int16 o;
  if(!BCD || !D) {
    o = A + i + C;
    V = ~(A ^ i) & (A ^ o) & 0x80;
  } else {
    idle();
    o = (A & 0x0f) + (i & 0x0f) + (C << 0);
    if(o > 0x09) o += 0x06;
    C = o > 0x0f;
    o = (A & 0xf0) + (i & 0xf0) + (C << 4) + (o & 0x0f);
    if(o > 0x9f) o += 0x60;
  }
  C = o.bit(8);
  Z = uint8(o) == 0;
  N = o.bit(7);
  return o;
}

auto MOS6502::AND(uint8 i) -> uint8 {
  uint8 o = A & i;
  Z = o == 0;
  N = o.bit(7);
  return o;
}

auto MOS6502::ASL(uint8 i) -> uint8 {
  C = i.bit(7);
  i <<= 1;
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::BIT(uint8 i) -> uint8 {
  Z = (A & i) == 0;
  V = i.bit(6);
  N = i.bit(7);
  return A;
}

auto MOS6502::CMP(uint8 i) -> uint8 {
  uint9 o = A - i;
  C = !o.bit(8);
  Z = uint8(o) == 0;
  N = o.bit(7);
  return A;
}

auto MOS6502::CPX(uint8 i) -> uint8 {
  uint9 o = X - i;
  C = !o.bit(8);
  Z = uint8(o) == 0;
  N = o.bit(7);
  return X;
}

auto MOS6502::CPY(uint8 i) -> uint8 {
  uint9 o = Y - i;
  C = !o.bit(8);
  Z = uint8(o) == 0;
  N = o.bit(7);
  return Y;
}

auto MOS6502::DEC(uint8 i) -> uint8 {
  i--;
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::EOR(uint8 i) -> uint8 {
  uint8 o = A ^ i;
  Z = o == 0;
  N = o.bit(7);
  return o;
}

auto MOS6502::INC(uint8 i) -> uint8 {
  i++;
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::LD(uint8 i) -> uint8 {
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::LSR(uint8 i) -> uint8 {
  C = i.bit(0);
  i >>= 1;
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::ORA(uint8 i) -> uint8 {
  uint8 o = A | i;
  Z = o == 0;
  N = o.bit(7);
  return o;
}

auto MOS6502::ROL(uint8 i) -> uint8 {
  bool c = C;
  C = i.bit(7);
  i = i << 1 | c;
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::ROR(uint8 i) -> uint8 {
  bool c = C;
  C = i.bit(0);
  i = c << 7 | i >> 1;
  Z = i == 0;
  N = i.bit(7);
  return i;
}

auto MOS6502::SBC(uint8 i) -> uint8 {
  i ^= 0xff;
  int16 o;
  if(!BCD || !D) {
    o = A + i + C;
    V = ~(A ^ i) & (A ^ o) & 0x80;
  } else {
    idle();
    o = (A & 0x0f) + (i & 0x0f) + (C << 0);
    if(o <= 0x0f) o -= 0x06;
    C = o > 0x0f;
    o = (A & 0xf0) + (i & 0xf0) + (C << 4) + (o & 0x0f);
    if(o <= 0xff) o -= 0x60;
  }
  C = o.bit(8);
  Z = uint8(o) == 0;
  N = o.bit(7);
  return o;
}

//

auto MOS6502::ALR(uint8 i) -> uint8 {
  return LSR(AND(i));
}

auto MOS6502::ANC(uint8 i) -> uint8 {
  uint8 o = AND(i);
  C = N;
  return o;
}

auto MOS6502::ARR(uint8 i) -> uint8 {
  uint8 o = A & i;
  o = C << 7 | o >> 1;
  C = o.bit(6);
  Z = o == 0;
  V = o.bit(6) ^ o.bit(5);
  N = o.bit(7);
  return o;
}

auto MOS6502::AXS(uint8 i) -> uint8 {
  uint9 o = (A & X) - i;
  C = !o.bit(8);
  Z = uint8(o) == 0;
  N = o.bit(7);
  return o;
}

auto MOS6502::DCP(uint8 i) -> uint8 {
  uint8 o = DEC(i);
  A = CMP(o);
  return o;
}

auto MOS6502::ISC(uint8 i) -> uint8 {
  uint8 o = INC(i);
  A = SBC(o);
  return o;
}

auto MOS6502::RLA(uint8 i) -> uint8 {
  uint8 o = ROL(i);
  A = AND(o);
  return o;
}

auto MOS6502::RRA(uint8 i) -> uint8 {
  uint8 o = ROR(i);
  A = ADC(o);
  return o;
}

auto MOS6502::SLO(uint8 i) -> uint8 {
  uint8 o = ASL(i);
  A = ORA(o);
  return o;
}

auto MOS6502::SRE(uint8 i) -> uint8 {
  uint8 o = LSR(i);
  A = EOR(o);
  return o;
}

auto MOS6502::XAA(uint8 i) -> uint8 {
  //XAA/ANE should be used with an argument of 0x00 (xaa #$00) or when the
  //accumulator is 0xff prior to execution (lda #$ff...xaa #$??). All other
  //combinations are subject to corruption.
  //http://csdb.dk/release/?id=143981

  return (A | xaaNoise) & X & i;
}

//

auto MOS6502::instructionAbsoluteModify(fp alu) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  auto data = read(absolute);
  write(absolute, data);
L write(absolute, ALU(data));
}

auto MOS6502::instructionAbsoluteModify(fp alu, uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageAlways(absolute, absolute + index);
  auto data = read(absolute + index);
  write(absolute + index, data);
L write(absolute + index, ALU(data));
}

auto MOS6502::instructionAbsoluteRead(fp alu, uint8& data) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
L data = ALU(read(absolute));
}

auto MOS6502::instructionAbsoluteRead(fp alu, uint8& data, uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageCrossed(absolute, absolute + index);
L data = ALU(read(absolute + index));
}

auto MOS6502::instructionAbsoluteWrite(uint8& data) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
L write(absolute, data);
}

auto MOS6502::instructionAbsoluteWrite(uint8& data, uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageAlways(absolute, absolute + index);
L write(absolute + index, data);
}

auto MOS6502::instructionBranch(bool take) -> void {
  if(!take) {
  L operand();
  } else {
    int8 displacement = operand();
    idlePageCrossed(PC, PC + displacement);
  L idle();
    PC = PC + displacement;
  }
}

auto MOS6502::instructionClear(bool& flag) -> void {
L idle();
  flag = 0;
}

auto MOS6502::instructionImmediate(fp alu, uint8& data) -> void {
L data = ALU(operand());
}

auto MOS6502::instructionImplied(fp alu, uint8& data) -> void {
L idle();
  data = ALU(data);
}

auto MOS6502::instructionIndirectXRead(fp alu, uint8& data) -> void {
  auto zeroPage = operand();
  load(zeroPage);
  uint16 absolute = load(zeroPage + X + 0);
  absolute |= load(zeroPage + X + 1) << 8;
L data = ALU(read(absolute));
}

auto MOS6502::instructionIndirectXWrite(uint8& data) -> void {
  auto zeroPage = operand();
  load(zeroPage);
  uint16 absolute = load(zeroPage + X + 0);
  absolute |= load(zeroPage + X + 1) << 8;
L write(absolute, data);
}

auto MOS6502::instructionIndirectYRead(fp alu, uint8& data) -> void {
  auto zeroPage = operand();
  uint16 absolute = load(zeroPage + 0);
  absolute |= load(zeroPage + 1) << 8;
  idlePageCrossed(absolute, absolute + Y);
L data = ALU(read(absolute + Y));
}

auto MOS6502::instructionIndirectYWrite(uint8& data) -> void {
  auto zeroPage = operand();
  uint16 absolute = load(zeroPage + 0);
  absolute |= load(zeroPage + 1) << 8;
  idlePageAlways(absolute, absolute + Y);
L write(absolute + Y, data);
}

auto MOS6502::instructionPull(uint8& data) -> void {
  idle();
  idle();
L data = pull();
  Z = data == 0;
  N = data.bit(7);
}

auto MOS6502::instructionPush(uint8& data) -> void {
  idle();
L push(data);
}

auto MOS6502::instructionSet(bool& flag) -> void {
L idle();
  flag = 1;
}

auto MOS6502::instructionTransfer(uint8& source, uint8& target, bool flag) -> void {
L idle();
  target = source;
  if(!flag) return;
  Z = target == 0;
  N = target.bit(7);
}

auto MOS6502::instructionZeroPageModify(fp alu) -> void {
  auto zeroPage = operand();
  auto data = load(zeroPage);
  store(zeroPage, data);
L store(zeroPage, ALU(data));
}

auto MOS6502::instructionZeroPageModify(fp alu, uint8 index) -> void {
  auto zeroPage = operand();
  load(zeroPage);
  auto data = load(zeroPage + index);
  store(zeroPage + index, data);
L store(zeroPage + index, ALU(data));
}

auto MOS6502::instructionZeroPageRead(fp alu, uint8& data) -> void {
  auto zeroPage = operand();
L data = ALU(load(zeroPage));
}

auto MOS6502::instructionZeroPageRead(fp alu, uint8& data, uint8 index) -> void {
  auto zeroPage = operand();
  load(zeroPage);
L data = ALU(load(zeroPage + index));
}

auto MOS6502::instructionZeroPageWrite(uint8& data) -> void {
  auto zeroPage = operand();
L store(zeroPage, data);
}

auto MOS6502::instructionZeroPageWrite(uint8& data, uint8 index) -> void {
  auto zeroPage = operand();
  read(zeroPage);
L store(zeroPage + index, data);
}

//

auto MOS6502::instructionIndirectXModify(fp alu) -> void {
  auto zeroPage = operand();
  load(zeroPage);
  uint16 absolute = load(zeroPage + X + 0);
  absolute |= load(zeroPage + X + 1) << 8;
  auto data = read(absolute);
  write(absolute, data);
L write(absolute, ALU(data));
}

auto MOS6502::instructionIndirectYModify(fp alu) -> void {
  auto zeroPage = operand();
  uint16 absolute = load(zeroPage + 0);
  absolute |= load(zeroPage + 1) << 8;
  idlePageAlways(absolute, absolute + Y);
  auto data = read(absolute + Y);
  write(absolute + Y, data);
L write(absolute + Y, ALU(data));
}

//

auto MOS6502::instructionBRK() -> void {
  operand();
  push(PCH);
  push(PCL);
  uint16 vector = 0xfffe;
  nmi(vector);
  push(P | 0x30);
  PCL = read(vector++);
  I = 1;
L PCH = read(vector++);
}

auto MOS6502::instructionJMPAbsolute() -> void {
  uint16 absolute = operand();
L absolute |= operand() << 8;
  PC = absolute;
}

auto MOS6502::instructionJMPIndirect() -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  uint16 pc = read(absolute);
L pc |= read((absolute & 0xff00) | ((absolute + 1) & 0x00ff)) << 8;
  PC = pc;
}

auto MOS6502::instructionJSRAbsolute() -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idle();
  PC--;
  push(PCH);
L push(PCL);
  PC = absolute;
}

auto MOS6502::instructionNOP() -> void {
L idle();
}

auto MOS6502::instructionPHP() -> void {
  idle();
L push(P | 0x30);
}

auto MOS6502::instructionPLP() -> void {
  idle();
  idle();
L P = pull();
}

auto MOS6502::instructionRTI() -> void {
  idle();
  idle();
  P = pull();
  PCL = pull();
L PCH = pull();
}

auto MOS6502::instructionRTS() -> void {
  idle();
  idle();
  PCL = pull();
  PCH = pull();
L idle();
  PC++;
}

//

auto MOS6502::instructionLASAbsolute(uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageCrossed(absolute, absolute + index);
L A = X = S = read(absolute + index) & S;
}

auto MOS6502::instructionLAXAbsolute() -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
L A = X = LD(read(absolute));
}

auto MOS6502::instructionLAXAbsolute(uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageCrossed(absolute, absolute + index);
L A = X = LD(read(absolute + index));
}

auto MOS6502::instructionLAXIndirectX() -> void {
  auto zeroPage = operand();
  load(zeroPage);
  uint16 absolute = load(zeroPage + X + 0);
  absolute |= load(zeroPage + X + 1) << 8;
L A = X = LD(read(absolute));
}

auto MOS6502::instructionLAXIndirectY() -> void {
  auto zeroPage = operand();
  uint16 absolute = load(zeroPage + 0);
  absolute |= load(zeroPage + 1) << 8;
  idlePageCrossed(absolute, absolute + Y);
L A = X = LD(read(absolute + Y));
}

auto MOS6502::instructionLAXZeroPage() -> void {
  auto zeroPage = operand();
L A = X = LD(load(zeroPage));
}

auto MOS6502::instructionLAXZeroPage(uint8 index) -> void {
  auto zeroPage = operand();
  load(zeroPage);
L A = X = LD(load(zeroPage + index));
}

auto MOS6502::instructionLXAImmediate() -> void {
  //While other LAX addressing modes decode to LDA and LDX, this one decodes
  //to LDA, LDX, and TAX, causing line noise on the data bus to interfere.
  //http://atariage.com/forums/topic/168616-lxa-stable/

  //LXA/ATX/OAL is safe to use if the argument is 0x00 (lxa #$00) or if the
  //accumulator is 0xff prior to execution (lda #$ff...lxa #$??). All other
  //combinations are subject to corruption.
  //http://csdb.dk/release/?id=143981

  //blargg's and hex_usr's NES consoles both run LXA as if it is LAX,
  //(noise is 0xff on both), but that can vary based on temperature and other
  //things.

L A = X = LD((A | lxaNoise) & operand());
}

auto MOS6502::instructionNOPAbsolute() -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
L idle();
}

auto MOS6502::instructionNOPAbsolute(uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageCrossed(absolute, absolute + index);
L idle();
}

auto MOS6502::instructionNOPImmediate() -> void {
L operand();
}

auto MOS6502::instructionNOPZeroPage() -> void {
  auto zeroPage = operand();
L idle();
}

auto MOS6502::instructionNOPZeroPage(uint8 index) -> void {
  auto zeroPage = operand();
  load(zeroPage);
L idle();
}

auto MOS6502::instructionSAXAbsolute() -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
L write(absolute, A & X);
}

auto MOS6502::instructionSAXIndirectX() -> void {
  auto zeroPage = operand();
  load(zeroPage);
  uint16 absolute = load(zeroPage + X + 0);
  absolute |= load(zeroPage + X + 1) << 8;
L write(absolute, A & X);
}

auto MOS6502::instructionSAXZeroPage() -> void {
  auto zeroPage = operand();
L store(zeroPage, A & X);
}

auto MOS6502::instructionSAXZeroPage(uint8 index) -> void {
  auto zeroPage = operand();
  read(zeroPage);
L store(zeroPage + index, A & X);
}

auto MOS6502::instructionSHAAbsolute(uint8 index) -> void {
  //Takes one fewer cycle than other AbsoluteModify instructions
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageAlways(absolute, absolute + index);
L write(absolute + index, A & X & (absolute.byte(1) + 1));
}

auto MOS6502::instructionSHAIndirectY() -> void {
  auto zeroPage = operand();
  uint16 absolute = load(zeroPage + 0);
  absolute |= load(zeroPage + 1) << 8;
  idlePageAlways(absolute, absolute + Y);
L write(absolute + Y, A & X & (absolute.byte(1) + 1));
}

auto MOS6502::instructionSHXAbsolute(uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageAlways(absolute, absolute + index);
  if(absolute.byte(1) != (absolute + index) >> 8) absolute.byte(1) &= r.x;
L write(absolute + index, X & (absolute.byte(1) + 1));
}

auto MOS6502::instructionSHYAbsolute(uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageAlways(absolute, absolute + index);
  if(absolute.byte(1) != (absolute + index) >> 8) absolute.byte(1) &= r.y;
L write(absolute + index, Y & (absolute.byte(1) + 1));
}

auto MOS6502::instructionSTP() -> void {
  operand();
  PC = 0xffff;
  r.mdr = 0xff;
  while(true) idle();
}

auto MOS6502::instructionTASAbsolute(uint8 index) -> void {
  uint16 absolute = operand();
  absolute |= operand() << 8;
  idlePageAlways(absolute, absolute + index);
  r.s = A & X;
L write(absolute + index, A & X & (absolute.byte(1) + 1));
}
