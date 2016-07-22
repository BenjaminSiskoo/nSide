auto M68K::testCondition(uint4 condition) -> bool {
  switch(condition) {
  case  0: return true;   //T
  case  1: return false;  //F
  case  2: return !r.c && !r.z;  //HI
  case  3: return  r.c ||  r.z;  //LS
  case  4: return !r.c;  //CC,HS
  case  5: return  r.c;  //CS,LO
  case  6: return !r.z;  //NE
  case  7: return  r.z;  //EQ
  case  8: return !r.v;  //VC
  case  9: return  r.v;  //VS
  case 10: return !r.n;  //PL
  case 11: return  r.n;  //MI
  case 12: return  r.n ==  r.v;  //GE
  case 13: return  r.n !=  r.v;  //LT
  case 14: return  r.n ==  r.v && !r.z;  //GT
  case 15: return  r.n !=  r.v ||  r.z;  //LE
  }
  unreachable;
}

//

template<> auto M68K::bits<Byte>() -> uint { return 8; }
template<> auto M68K::bits<Word>() -> uint { return 16; }
template<> auto M68K::bits<Long>() -> uint { return 32; }

template<> auto M68K::mask<Byte>() -> uint32 { return 0xff; }
template<> auto M68K::mask<Word>() -> uint32 { return 0xffff; }
template<> auto M68K::mask<Long>() -> uint32 { return 0xffffffff; }

template<> auto M68K::clip<Byte>(uint32 data) -> uint32 { return data & 0xff; }
template<> auto M68K::clip<Word>(uint32 data) -> uint32 { return data & 0xffff; }
template<> auto M68K::clip<Long>(uint32 data) -> uint32 { return data & 0xffffffff; }

template<> auto M68K::sign<Byte>(uint32 data) -> int32 { return  (int8)data; }
template<> auto M68K::sign<Word>(uint32 data) -> int32 { return (int16)data; }
template<> auto M68K::sign<Long>(uint32 data) -> int32 { return (int32)data; }

template<uint Size> auto M68K::carry(uint32 result, uint32 source) -> bool {
  return clip<Size>(result) < clip<Size>(source);
}

template<uint Size> auto M68K::overflow(uint32 result, uint32 source, uint32 target) -> bool {
  return sign<Size>((target ^ source) & (target ^ result)) < 0;
}

template<uint Size> auto M68K::zero(uint32 result) -> bool {
  return clip<Size>(result) == 0;
}

template<uint Size> auto M68K::negative(uint32 result) -> bool {
  return sign<Size>(result) < 0;
}

//

template<uint Size> auto M68K::instructionADD(Register rd, uint1 direction, EA ea) -> void {
  uint32 source;
  uint32 target;
  uint32 result;

  if(direction == 0) {
    source = read<Size>(ea);
    target = read<Size>(rd);
    result = source + target;
    write<Size>(rd, result);
  } else {
    source = read<Size>(rd);
    target = read<Size>(ea);
    result = source + target;
    write<Size>(ea, result);
  }

  r.c = carry<Size>(result, source);
  r.v = overflow<Size>(result, source, target);
  r.z = zero<Size>(result);
  r.n = negative<Size>(result);
  r.x = r.c;
}

template<uint Size> auto M68K::instructionANDI(EA ea) -> void {
  auto source = readPC<Size>();
  auto target = read<Size, NoUpdate>(ea);
  auto result = target & source;
  write<Size>(ea, result);

  r.c = 0;
  r.v = 0;
  r.z = zero<Size>(result);
  r.n = negative<Size>(result);
}

auto M68K::instructionBCC(uint4 condition, uint8 displacement) -> void {
  auto extension = readPC();
  if(condition == 1) push<Long>(r.pc);
  r.pc -= 2;
  if(condition >= 2 && !testCondition(condition)) return;  //0 = BRA; 1 = BSR
  r.pc += displacement ? sign<Byte>(displacement) : sign<Word>(extension);
}

template<uint Size> auto M68K::instructionBTST(Register rd, EA ea) -> void {
  auto bit = read<Size>(rd);
  auto test = read<Size>(ea);
  bit &= bits<Size>() - 1;

  r.z = test.bit(bit) == 0;
}

template<uint Size> auto M68K::instructionBTST(EA ea) -> void {
  auto bit = (uint8)readPC<Word>();
  auto test = read<Size>(ea);
  bit &= bits<Size>() - 1;

  r.z = test.bit(bit) == 0;
}

template<uint Size> auto M68K::instructionCLR(EA ea) -> void {
  read<Size>(ea);
  write<Size>(ea, 0);

  r.c = 0;
  r.v = 0;
  r.z = 1;
  r.n = 0;
}

template<uint Size> auto M68K::instructionCMP(Register rd, EA ea) -> void {
  auto source = read<Size>(ea);
  auto target = read<Size>(rd);
  auto result = target - source;

  r.c = carry<Size>(result, source);
  r.v = overflow<Size>(result, source, target);
  r.z = zero<Size>(result);
  r.n = negative<Size>(result);
}

auto M68K::instructionDBCC(uint4 condition, Register rd) -> void {
  auto displacement = (int16)readPC();
  if(!testCondition(condition)) {
    uint16 result = read<Word>(rd);
    write<Word>(rd, result - 1);
    if(result) r.pc -= 2, r.pc += displacement;
  }
}

auto M68K::instructionLEA(Register ra, EA ea) -> void {
  write<Long>(ra, fetch<Long>(ea));
}

template<uint Size> auto M68K::instructionMOVE(EA to, EA from) -> void {
  auto data = read<Size>(from);
  write<Size>(to, data);

  r.c = 0;
  r.v = 0;
  r.z = zero<Size>(data);
  r.n = negative<Size>(data);
}

template<uint Size> auto M68K::instructionMOVEA(Register ra, EA ea) -> void {
  auto data = read<Size>(ea);
  if(Size == Word) data = (int16)data;
  write<Long>(ra, data);
}

template<uint Size> auto M68K::instructionMOVEM(uint1 direction, EA ea) -> void {
  auto list = readPC();
  auto addr = fetch<Size>(ea);

  for(uint rn : range(16)) {
    if(list.bit(rn)) {
      write<Size>(Register{rn}, read<Size>(addr));
      addr += Size == Long ? 4 : 2;
    }
  }

  flush<Size>(ea, addr);
}

auto M68K::instructionMOVEQ(Register rd, uint8 immediate) -> void {
  write<Byte>(rd, immediate);

  r.c = 0;
  r.v = 0;
  r.z = zero<Byte>(immediate);
  r.n = negative<Byte>(immediate);
}

auto M68K::instructionMOVE_FROM_SR(EA ea) -> void {
  write<Word>(ea, r.sr);
}

auto M68K::instructionMOVE_TO_SR(EA ea) -> void {
  if(!supervisor()) return;

  setSR(read<Word>(ea));
}

auto M68K::instructionMOVE_USP(uint1 direction, Register ra) -> void {
  if(!supervisor()) return;

  if(direction == 0) {
    r.sp = read<Long>(ra);
  } else {
    write<Long>(ra, r.sp);
  }
}

auto M68K::instructionNOP() -> void {
}

auto M68K::instructionRTS() -> void {
  r.pc = pop<Long>();
}

template<uint Size> auto M68K::instructionTST(EA ea) -> void {
  auto data = read<Size>(ea);

  r.c = 0;
  r.v = 0;
  r.z = zero<Size>(data);
  r.n = negative<Size>(data);
}
