struct Flags {
  union {
    uint8_t b = 0;
    BooleanBitField<uint8_t, 7> n;
    BooleanBitField<uint8_t, 6> v;
    BooleanBitField<uint8_t, 3> d;
    BooleanBitField<uint8_t, 2> i;
    BooleanBitField<uint8_t, 1> z;
    BooleanBitField<uint8_t, 0> c;
  };

  inline operator uint() const { return b; }
  inline auto operator =(uint value) -> uint { return b  = value; }
  inline auto operator&=(uint value) -> uint { return b &= value; }
  inline auto operator|=(uint value) -> uint { return b |= value; }
  inline auto operator^=(uint value) -> uint { return b ^= value; }
};

struct Reg16 {
  union {
    uint16_t w;
    NaturalBitField<uint16_t, 0,  7> l;
    NaturalBitField<uint16_t, 8, 15> h;
  };

  inline operator uint() const { return w; }
  inline auto operator  =(uint value) -> uint { return w   = value; }
  inline auto operator |=(uint value) -> uint { return w  |= value; }
  inline auto operator ^=(uint value) -> uint { return w  ^= value; }
  inline auto operator &=(uint value) -> uint { return w  &= value; }
  inline auto operator<<=(uint value) -> uint { return w <<= value; }
  inline auto operator>>=(uint value) -> uint { return w >>= value; }
  inline auto operator +=(uint value) -> uint { return w  += value; }
  inline auto operator -=(uint value) -> uint { return w  -= value; }
  inline auto operator *=(uint value) -> uint { return w  *= value; }
  inline auto operator /=(uint value) -> uint { return w  /= value; }
  inline auto operator %=(uint value) -> uint { return w  %= value; }
};

struct Registers {
  uint8 mdr;
  Reg16 pc;
  uint8 a, x, y, s;
  Flags p;
};
