struct Background {
  Background(uint id) : id(id) {}

  auto power() -> void;

  alwaysinline auto getTile(uint x, uint y) -> uint16;

  auto scanline() -> void;
  alwaysinline auto directColor(uint8 palette, uint16 tile) const -> uint16;
  auto render() -> void;
  alwaysinline auto renderMode7() -> void;

  auto serialize(serializer&) -> void;

  struct ID { enum : uint { BG1, BG2, BG3, BG4 }; };
  const uint id;

  struct Mode { enum : uint { BPP2, BPP4, BPP8, Mode7, Inactive }; };
  struct ScreenSize { enum : uint { Size32x32, Size32x64, Size64x32, Size64x64 }; };
  struct TileSize { enum : uint { Size8x8, Size16x16 }; };
  struct Screen { enum : uint { Above, Below }; };

  struct IO {
    uint16 tiledataAddress;
    uint16 screenAddress;
    uint2 screenSize;
    uint4 mosaic;
    bool tileSize;

    uint mode;
    uint priority[2];

    bool aboveEnable;
    bool belowEnable;

    uint16 hoffset;
    uint16 voffset;
  } io;

  struct {
    //int x;
    int y;

    uint tile;
    uint priority;
    uint paletteNumber;
    uint paletteIndex;
  };

  struct {
    uint16 hoffset, voffset;

    uint16 a, b, c, d, x, y;
  } m7cache;

  uint16 tileWidth, tileHeight;
  uint16 maskX,     maskY;
  uint16 screenX,   screenY;

  friend class PPU;
};
