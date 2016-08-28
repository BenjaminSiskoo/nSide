//Television Interface Adaptor

struct TIA : Thread {
  shared_pointer<Emulator::Stream> stream;

  TIA();
  ~TIA();

  alwaysinline auto step(uint clocks) -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;

  //io.cpp
  auto readIO(uint6 addr, uint8 data) -> uint8;
  auto writeIO(uint6 addr, uint8 data) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  struct IO {
    uint8 hcounter;
    uint vcounter;  //not actually in TIA; needed for rendering

    //$0000 VSYNC
    bool vsync;

    //$0001 VBLANK
    bool vblank;

    //$0006 COLUP0
    uint7 playerMissile0Color;

    //$0007 COLUP1
    uint7 playerMissile1Color;

    //$0008 COLUPF
    uint7 playfieldBallColor;

    //$0009 COLUBK
    uint7 backgroundColor;

    //$000a CTRLPF
    bool playfieldBallPriority;
  } io;

  struct Player {
    uint3 numberSize;  //also affects missiles
    bool reflect;
    uint8 graphic;
    uint8 position;
    int3 motion;
  } player[2];

  struct Missile {
    bool enable;
    uint2 size;
    uint8 position;
    int3 motion;
    bool reset;
  } missile[2];

  struct Ball {
    bool enable;
    uint2 size;
    uint8 position;
    int3 motion;
  } ball;

  struct Playfield {
    bool reflect;
    bool score;

    uint4 graphic0;
    uint8 graphic1;
    uint8 graphic2;
  } playfield;

  struct Collision {
    bool m0p1;
    bool m0p0;
    bool m1p0;
    bool m1p1;
    bool p0pf;
    bool p0bl;
    bool p1pf;
    bool p1bl;
    bool m0pf;
    bool m0bl;
    bool m1pf;
    bool m1bl;
    bool blpf;
    bool p0p1;
    bool m0m1;
  } collision;

privileged:
  alwaysinline auto hblank() -> bool { return io.hcounter < 68; }

  auto run() -> void;

  auto scanline() -> void;
  auto frame() -> void;
  auto refresh() -> void;

  friend class System;

  uint32* output = nullptr;
};

extern TIA tia;
