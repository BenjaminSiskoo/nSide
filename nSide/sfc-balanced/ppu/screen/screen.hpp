struct Screen {
  auto reset() -> void;

  auto blend(uint x, uint y) const -> uint16;

  auto serialize(serializer&) -> void;

  struct ID { enum : uint { BACK = 5 }; };

  struct Registers {
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
  } r;

  struct Math {
    //struct Screen {
    //  uint16 color;
    //  bool colorEnable;
    //} above, below;
    //bool transparent;
    //bool blendMode;
    bool colorHalve;
  } math;

  friend class PPU;
};
