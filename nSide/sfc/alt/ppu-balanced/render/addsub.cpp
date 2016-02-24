//color addition / subtraction
//thanks go to blargg for the optimized algorithms
inline auto PPU::addsub(uint32_t x, uint32_t y, bool halve) -> uint16_t {
  if(!regs.color_mode) {
    if(!halve) {
      uint_t sum = x + y;
      uint_t carry = (sum - ((x ^ y) & 0x0421)) & 0x8420;
      return (sum - carry) | (carry - (carry >> 5));
    } else {
      return (x + y - ((x ^ y) & 0x0421)) >> 1;
    }
  } else {
    uint_t diff = x - y + 0x8420;
    uint_t borrow = (diff - ((x ^ y) & 0x8420)) & 0x8420;
    if(!halve) {
      return   (diff - borrow) & (borrow - (borrow >> 5));
    } else {
      return (((diff - borrow) & (borrow - (borrow >> 5))) & 0x7bde) >> 1;
    }
  }
}
