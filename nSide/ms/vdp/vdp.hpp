//Texas Instruments TMS9918A

struct VDP : Thread {
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;

  auto power() -> void;
  auto reset() -> void;

  auto vblank() -> bool;

  //io.cpp
  auto readData() -> uint8;
  auto writeData(uint8 data) -> void;

  auto readControl() -> uint8;
  auto writeControl(uint8 data) -> void;

private:
  uint8 vram[16 * 1024];
  uint8 cram[64];  //SG-1000: 0, MS: 32, GG: 64

  struct IO {
    uint14 controlAddress;
    uint2  controlCode;
    bool   controlLatch;

    uint5 fifthSprite;     //Modes 1, 2, 3
    bool spriteCollision;
    bool spriteOverflow;   //Mode 4
  } io;
};

extern VDP vdp;
