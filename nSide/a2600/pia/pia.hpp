//MOS 6532 RIOT

struct PIA : Thread {
  PIA();
  ~PIA();

  alwaysinline auto step(uint clocks) -> void;

  static auto Enter() -> void;
  auto main() -> void;
  auto load(Markup::Node) -> bool;
  auto power() -> void;

  //memory.cpp
  auto readRAM(uint7 addr, uint8 data) -> uint8;
  auto writeRAM(uint7 addr, uint8 data) -> void;

  //io.cpp
  auto readIO(uint7 addr, uint8 data) -> uint8;
  auto writeIO(uint7 addr, uint8 data) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint8 ram[128];
  vector<Thread*> peripherals;

privileged:
  struct IO {
    bool color;
    bool difficulty0;
    bool difficulty1;

    //$0280 SWCHA
    uint8 swcha;

    //$0281 SWACNT
    uint8 swacnt;

    //$0282 SWCHB
    uint8 swchb;

    //$0283 SWBCNT
    uint8 swbcnt;

    uint18 timer;
    uint18 timerMask;
    bool timerInterrupt;
    bool pa7Interrupt;
    bool edgeDetect;
  } io;

  struct Input {
    bool color;
    bool difficulty0;
    bool difficulty1;
  } input;
};

extern PIA pia;
