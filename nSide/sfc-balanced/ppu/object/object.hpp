struct Object {
  alwaysinline auto addressReset() -> void;
  alwaysinline auto setFirstSprite() -> void;
  auto reset() -> void;

  struct Sprite;

  //list.cpp
  auto update(uint addr, uint8 data) -> void;

  auto serialize(serializer&) -> void;

  struct ID { enum : uint { OBJ = 4 }; };

  struct Registers {
    bool aboveEnable;
    bool belowEnable;
    bool interlace;

    uint3  baseSize;
    uint2  nameSelect;
    uint16 tiledataAddress;
    uint7  firstSprite;

    uint priority[4];

    bool timeOver;
    bool rangeOver;
  } r;

  /*
  struct Item {
    bool  valid;
    uint7 index;
  };

  struct Tile {
    bool   valid;
    uint9  x;
    uint2  priority;
    uint8  palette;
    uint1  hflip;
    uint32 data;
  };
  */

  struct State {
    //uint x;
    //uint y;

    uint itemCount;
    uint tileCount;

    //bool active;
    //Item item[2][32];
    //Tile tile[2][34];
  } t;

  /*
  struct Output {
    struct Pixel {
      uint  priority;  //0 = none (transparent)
      uint8 palette;
    } above, below;
  } output;
  */

  struct Sprite {
    uint8 width, height;
    uint9 x;
    uint8 y;
    uint8 character;
    bool  nameSelect;
    bool  vflip;
    bool  hflip;
    uint2 priority;
    uint3 palette;
    bool  size;
  } list[128];

  friend class PPU;
};
