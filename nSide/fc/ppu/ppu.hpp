struct PPU : Thread, PPUcounter {
  enum class Version : uint {
    //YIQ
    RP2C02C,
    RP2C02G,

    //RGB
    RP2C03B,
    RP2C03G,
    RP2C04_0001,
    RP2C04_0002,
    RP2C04_0003,
    RP2C04_0004,
    RC2C03B,
    RC2C03C,
    RC2C05_01,
    RC2C05_02,
    RC2C05_03,
    RC2C05_04,
    RC2C05_05,

    //YUV
    RP2C07,
    UA6538,
  } version;

  alwaysinline auto ntsc() const -> bool { return version >= Version::RP2C02C && version <= Version::RP2C02G; }
  alwaysinline auto pal()  const -> bool { return version >= Version::RP2C07  && version <= Version::UA6538;  }
  alwaysinline auto rgb()  const -> bool { return version >= Version::RP2C03B && version <= Version::RC2C05_05; }

  PPU();
  ~PPU();

  alwaysinline auto step(uint clocks) -> void;
  alwaysinline auto synchronizeCPU() -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;
  auto reset() -> void;

  auto origin_x() -> uint;
  auto origin_y() -> uint;

  auto ext() -> uint4;

  auto serialize(serializer&) -> void;

  //memory.cpp
  auto readCIRAM(uint14 addr) -> uint8;
  auto writeCIRAM(uint14 addr, uint8 data) -> void;

  alwaysinline auto readCGRAM(uint5 addr) -> uint8;
  alwaysinline auto writeCGRAM(uint5 addr, uint8 data) -> void;

  alwaysinline auto readOAM(uint8 addr) -> uint8;
  alwaysinline auto writeOAM(uint8 addr, uint8 data) -> void;

  //mmio.cpp
  auto read(uint16 addr, uint8 data) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;

  //render.cpp
  auto enable() const -> bool;
  auto nametableAddress() const -> uint;
  auto scrollX() const -> uint;
  auto scrollY() const -> uint;

  auto loadCHR(uint14 addr) -> uint8;

  auto scrollX_increment() -> void;
  auto scrollY_increment() -> void;

  auto renderPixel() -> void;
  auto renderSprite() -> void;
  auto renderScanline() -> void;

  uint8 ciram[4096];  //2048 in Famicom and PlayChoice-10
  uint8 cgram[32];
  uint8 oam[256];

  bool side;  //VS. System; 0: main, 1: sub

  static const uint9 RP2C03[16 * 4];
  static const uint9 RP2C04_0001[16 * 4];
  static const uint9 RP2C04_0002[16 * 4];
  static const uint9 RP2C04_0003[16 * 4];
  static const uint9 RP2C04_0004[16 * 4];

  struct Registers {
    uint14 chrAddressBus;
    uint8 mdr;
    uint mdrDecay[8];

    uint8 busData;

    bool addressLatch;

    uint15 vaddr;
    uint15 taddr;
    uint8 xaddr;

    bool nmiHold;
    bool nmiFlag;

    //$2000  PPUCTRL
    bool nmiEnable;
    bool masterSelect;
    uint spriteHeight;
    uint bgAddress;
    uint objAddress;
    uint vramIncrement;

    //$2001  PPUMASK
    uint3 emphasis;
    bool objEnable;
    bool bgEnable;
    bool objEdgeEnable;
    bool bgEdgeEnable;
    bool grayscale;

    //$2002  PPUSTATUS
    bool spriteZeroHit;
    bool spriteOverflow;

    //$2003  OAMADDR
    uint8 oamAddress;
  } r;

  struct OAM {
    uint8 id;
    uint8 y;
    uint8 tile;
    uint8 attr;
    uint8 x;

    uint8 tiledataLo;
    uint8 tiledataHi;
  };

  struct Latches {
    uint16 nametable;
    uint16 attribute;
    uint16 tiledataLo;
    uint16 tiledataHi;

    uint oamIterator;
    uint oamCounter;

    OAM oam[8];   //primary
    OAM soam[8];  //secondary
  } l;

privileged:
  uint32* output = nullptr;

  alwaysinline auto addClocks(uint) -> void;

  auto scanline() -> void;
  auto frame() -> void;
  auto refresh() -> void;

  friend class Zapper;
  friend class BeamGun;
  friend class Scheduler;
};

extern PPU ppu;
