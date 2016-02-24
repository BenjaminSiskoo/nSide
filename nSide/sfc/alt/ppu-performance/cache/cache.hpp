struct Cache {
  Cache(PPU& self);

  auto tile_2bpp(uint_t tile) -> uint8_t*;
  auto tile_4bpp(uint_t tile) -> uint8_t*;
  auto tile_8bpp(uint_t tile) -> uint8_t*;
  auto tile(uint_t bpp, uint_t tile) -> uint8_t*;

  auto serialize(serializer&) -> void;

  PPU& self;
  uint8_t* tiledata[3];
  uint8_t* tilevalid[3];

  friend class PPU;
};
