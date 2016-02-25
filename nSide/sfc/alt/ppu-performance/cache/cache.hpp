struct Cache {
  Cache(PPU& self);

  auto tile_2bpp(uint tile) -> uint8_t*;
  auto tile_4bpp(uint tile) -> uint8_t*;
  auto tile_8bpp(uint tile) -> uint8_t*;
  auto tile(uint bpp, uint tile) -> uint8_t*;

  auto serialize(serializer&) -> void;

  PPU& self;
  uint8_t* tiledata[3];
  uint8_t* tilevalid[3];

  friend class PPU;
};
