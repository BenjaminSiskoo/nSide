struct Background {
  Background(uint id) : id(id) {}

  auto reset() -> void;

  auto getTile(uint x, uint y) -> uint;

  auto serialize(serializer&) -> void;

  struct ID { enum : uint { BG1, BG2, BG3, BG4 }; };
  const uint id;

  struct Mode { enum : uint { BPP2, BPP4, BPP8, Mode7, Inactive }; };
  struct ScreenSize { enum : uint { Size32x32, Size32x64, Size64x32, Size64x64 }; };
  struct TileSize { enum : uint { Size8x8, Size16x16 }; };
  struct Screen { enum : uint { Above, Below }; };

  struct Registers {
    uint16 tiledataAddress;
    uint16 screenAddress;
    uint2 screenSize;
    bool mosaicEnabled;
    bool tileSize;

    uint mode;
    uint priority[2];

    bool aboveEnable;
    bool belowEnable;

    uint16 hoffset;
    uint16 voffset;
  } r;

  friend class PPU;
};
