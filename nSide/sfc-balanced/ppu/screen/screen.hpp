struct Screen {
  auto reset() -> void;

  auto blend(uint x, uint y) const -> uint15;
  /*alwaysinline*/ auto paletteColor(uint8 palette) const -> uint15;
  /*alwaysinline*/ auto directColor(uint8 palette, uint16 tile) const -> uint15;

  auto serialize(serializer&) -> void;

  struct ID { enum : uint { BACK = 5 }; };

  uint15 cgram[256];

  struct IO {
    bool blendMode;
    bool directColor;

    bool colorMode;
    bool colorHalve;
    struct Layer {
      bool colorEnable;
    } bg1, bg2, bg3, bg4, obj, back;

    uint5 colorBlue;
    uint5 colorGreen;
    uint5 colorRed;
  } io;

  struct Math {
    //struct Screen {
    //  uint15 color;
    //  bool colorEnable;
    //} above, below;
    //bool transparent;
    //bool blendMode;
    bool colorHalve;
  } math;

  friend class PPU;
};
