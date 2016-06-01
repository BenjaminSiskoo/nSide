struct Background {
  struct ID { enum : uint { BG1, BG2, BG3, BG4 }; };
  const uint id;

  struct Mode { enum : uint { BPP2, BPP4, BPP8, Mode7, Inactive }; };
  struct ScreenSize { enum : uint { Size32x32, Size32x64, Size64x32, Size64x64 }; };
  struct TileSize { enum : uint { Size8x8, Size16x16 }; };
  struct Screen { enum : uint { Main, Sub }; };

  Background(PPU& self, uint id);

  struct Regs {
    uint16 tiledata_addr;
    uint16 screen_addr;
    uint2 screen_size;
    bool mosaic_enabled;
    bool tile_size;

    uint mode;//
    uint priority0;//
    uint priority1;//

    bool main_enable;
    bool sub_enable;

    uint16 hoffset;
    uint16 voffset;
  } regs;

  auto serialize(serializer&) -> void;

  PPU& self;
  friend class PPU;
};
