struct VSSystem : Thread {
  enum : uint {
    Button1, Button2, Button3, Button4, ServiceButton, Coin1, Coin2,
  };

  struct GameCount { enum : uint {
    Uni = 1,
    Dual = 2,
  };};

  static auto Enter() -> void;
  auto main() -> void;

  auto init() -> void;
  auto load() -> bool;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto setDip(bool side, uint8 dip) -> void;
  auto read(bool side, uint16 addr, uint8 data) -> uint8;
  auto write(bool side, uint16 addr, uint8 data) -> void;

  auto serialize(serializer& s) -> void;

  bool forceSubRAM;
  uint gameCount;

private:
  uint8 ram[0x800];
  bool ramSide; // 0: main, 1: sub

  bool swapControllers;
  uint8 mainDip;
  uint8 subDip;
  uint watchdog;

  //control.cpp
  bool mainControlLatched;
  uint mainControlCounter1;
  uint mainControlCounter2;
  bool mainButtons[4];
  bool subControlLatched;
  uint subControlCounter1;
  uint subControlCounter2;
  bool subButtons[4];

  auto resetButtons() -> void;
  auto poll(bool side, uint input) -> int16;
  auto data1(bool side) -> bool;
  auto data2(bool side) -> bool;
  auto latch(bool side, bool data) -> void;

  friend class Cartridge;
};

extern VSSystem vssystem;
