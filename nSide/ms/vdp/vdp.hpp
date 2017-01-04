//TI TMS9918A (derivative)

struct VDP : Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;
  auto refresh() -> void;

  auto power() -> void;
  auto reset() -> void;

  //io.cpp
  auto vcounter() -> uint8;
  auto hcounter() -> uint8;
  auto data() -> uint8;
  auto status() -> uint8;

  auto data(uint8) -> void;
  auto control(uint8) -> void;
  auto registerWrite(uint4 addr, uint8 data) -> void;

  //render.cpp
  auto run() -> void;
  auto outputPixel(uint12 color) -> void;

private:
  inline auto screenX() -> uint;
  inline auto screenY() -> uint;
  inline auto screenWidth() -> uint;
  inline auto screenHeight() -> uint;
  inline auto activeWidth() -> uint;
  inline auto activeHeight() -> uint;

  uint32 buffer[256 * 240];  //SG-1000: 256 * 192
  uint8 vram[0x4000];
  uint8 cram[0x40];  //SG-1000: 0, MS: 32, GG: 64

  struct IO {
    uint vcounter;
    uint hcounter;

    bool controlLatch;
    uint16 controlData;
    uint2 code;
    uint14 address;

    uint8 vramLatch;

    //$00  mode control 1
    bool externalSync;
    bool extendedHeight;
    bool mode4;
    bool spriteShift;
    bool lineInterrupts;
    bool leftClip;
    bool horizontalScrollLock;
    bool verticalScrollLock;

    //$01  mode control 2
    bool spriteDouble;
    bool spriteTile;
    bool lines240;
    bool lines224;
    bool frameInterrupts;
    bool displayEnable;

    //$02  name table base address
    uint1 nameTableMask;
    uint3 nameTableAddress;

    //$03  color table base address
    uint8 colorTableAddress;

    //$04  pattern table base address
    uint8 patternTableAddress;

    //$05  sprite attribute table base address
    uint1 spriteAttributeTableMask;
    uint6 spriteAttributeTableAddress;

    //$06  sprite pattern table base address
    uint2 spritePatternTableMask;
    uint1 spritePatternTableAddress;

    //$07  backdrop color
    uint4 backdropColor;

    //$08  horizontal scroll offset
    uint8 hscroll;

    //$09  vertical scroll offset
    uint8 vscroll;

    //$0a  line counter
    uint8 lineCounter;
  } io;
};

extern VDP vdp;
