//Hudson Soft HuC6260 -- Video Color Encoder
//Hudson Soft HuC6270 -- Video Display Controller

struct VDC : Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;
  auto refresh() -> void;

  auto power() -> void;

  //io.cpp
  auto vramRead(uint16 addr) -> uint16;
  auto vramWrite(uint16 addr, uint16 data) -> void;

  auto read(uint11 addr) -> uint8;
  auto write(uint11 addr, uint8 data) -> void;

private:
  uint32 buffer[512 * 484];

  uint16 vram[0x8000];
  uint16 satb[0x100];
  uint9 cram[0x200];

  struct State {
    uint x;
    uint y;
  } state;

  struct IRQ {
    enum class Line : uint {
      Collision,
      Overflow,
      LineCoincidence,
      Vblank,
      TransferVRAM,
      TransferSATB,
    };

    //irq.cpp
    auto poll() -> void;
    auto raise(Line) -> void;
    auto lower() -> void;

    bool enableCollision;
    bool enableOverflow;
    bool enableLineCoincidence;
    bool enableVblank;
    bool enableTransferVRAM;
    bool enableTransferSATB;

    bool pendingCollision;
    bool pendingOverflow;
    bool pendingLineCoincidence;
    bool pendingVblank;
    bool pendingTransferVRAM;
    bool pendingTransferSATB;
  } irq;

  struct DMA {
    //dma.cpp
    auto step(uint clocks) -> void;
    auto vramStart() -> void;
    auto satbStart() -> void;
    auto satbQueue() -> void;

    bool   sourceIncrementMode;
    bool   targetIncrementMode;
    bool   satbRepeat;
    uint16 source;
    uint16 target;
    uint16 length;
    uint16 satbSource;

    bool   vramActive;
    bool   satbActive;
    bool   satbPending;
    uint16 satbOffset;
  } dma;

  struct Background {
    //background.cpp
    auto scanline(uint y) -> void;
    auto run(uint x, uint y) -> void;

    bool   enable;
    uint10 hscroll;
    uint9  vscroll;
    uint8  width;
    uint8  height;

    uint10 hoffset;
    uint9  voffset;

    maybe<uint9> color;
  } background;

  struct Sprite {
    //sprite.cpp
    auto scanline(uint y) -> void;
    auto run(uint x, uint y) -> void;

    bool enable;

    struct Object {
      uint10 y;
      uint10 x;
      bool   mode;
      uint10 pattern;
      uint4  palette;
      bool   priority;
      uint   width;
      bool   hflip;
      uint   height;
      bool   vflip;
    };
    array<Object, 64> objects;

    maybe<uint9> color;
    bool priority;
  } sprite;

  struct IO {
    uint5  address;

    //VDC

    //$00  MAWR (W)
    uint16 vramAddressWrite;

    //$01  MARR (W)
    uint16 vramAddressRead;

    //$02  VWR (W)
    //$02  VRR (R)
    uint16 vramDataWrite;
    uint16 vramDataRead;

    //$05  CR (W)
    uint2  externalSync;
    uint2  displayOutput;
    bool   dramRefresh;
    uint   vramAddressIncrement;

    //$06  RCR
    uint10 lineCoincidence;

    //$09  MWR
    uint2  vramAccess;
    uint2  spriteAccess;
    bool   cgMode;

    //$0a  HSR
    uint5  horizontalSyncWidth;
    uint7  horizontalDisplayStart;

    //$0b  HDR
    uint7  horizontalDisplayWidth;
    uint7  horizontalDisplayEnd;

    //$0c  VPR
    uint5  verticalSyncWidth;
    uint8  verticalDisplayStart;

    //$0d  VDR
    uint9  verticalDisplayWidth;

    //$0e  VCR
    uint8  verticalDisplayEnd;

    //VCE

    //$00  CR
    uint2  divisionRatio;
    bool   colorBlur;
    bool   grayscale;

    //$02  CTA
    uint9  colorAddress;
  } io;
};

extern VDC vdc;
