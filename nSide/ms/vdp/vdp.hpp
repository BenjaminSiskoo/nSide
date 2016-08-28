//TI TMS9918A (derivative)

struct VDP : Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;
  auto refresh() -> void;

  auto in(uint8 addr) -> uint8;
  auto out(uint8 addr, uint8 data) -> void;

  auto power() -> void;
  auto reset() -> void;

  auto vblank() -> bool;

  //io.cpp
  auto readData() -> uint8;
  auto writeData(uint8 data) -> void;

  auto readControl() -> uint8;
  auto writeControl(uint8 data) -> void;

  //render.cpp
  auto scanline() -> void;
  auto run() -> void;
  auto outputPixel(uint12 color) -> void;

private:
  inline auto screenX() -> uint;
  inline auto screenY() -> uint;
  inline auto screenWidth() -> uint;
  inline auto screenHeight() -> uint;
  inline auto activeWidth() -> uint;
  inline auto activeHeight() -> uint;

  uint8 vram[16 * 1024];
  uint8 cram[64];  //SG-1000: 0, MS: 32, GG: 64

  struct IO {
    uint14 controlAddress;
    uint2 controlCode;
    bool controlLatch;

    bool externalVDP;
    bool mode3;
    bool mode4;
    bool spriteShiftLeft;
    bool lineInterrupt;
    bool hideLeft;
    bool horizontalLock;
    bool verticalLock;

    bool spriteMag;
    bool spriteSize;
    bool mode2;
    bool mode1;
    bool interrupt;
    bool blank;
    bool ramSize;

    bool nametableMask;
    uint14 nametableAddress;

    uint14 colorAddress;

    uint14 patternAddress;

    uint14 spriteAttributeMask;
    uint14 spriteAttributeAddress;

    uint14 spritePatternMask;
    uint14 spritePatternAddress;

    uint4 backColor;
    uint4 textColor;

    uint8 scrollX;

    uint8 scrollY;

    uint8 lineCounter;

    uint5 fifthSprite;
    bool spriteCollision;
    bool spriteOverflow;
  } io;

  struct State {
    uint32* output = nullptr;
    uint x;
    uint y;
  } state;

  uint32 buffer[256 * 240];  //SG-1000: 256 * 192
};

extern VDP vdp;
