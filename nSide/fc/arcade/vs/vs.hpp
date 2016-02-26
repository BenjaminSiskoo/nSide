struct VSArcadeBoard {
  VSPanel panel{3};
  bool swap_controllers;

  auto init() -> void;
  auto load() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto set_dip(bool side, uint8 dip) -> void;
  auto read(bool side, uint16 addr, uint8 data) -> uint8;
  auto write(bool side, uint16 addr, uint8 data) -> void;

  auto serialize(serializer& s) -> void;

private:
  uint8 ram[2048];
  bool ramSide; // 0: main, 1: sub
  uint8 dip;
  uint watchdog;

  auto r4016(bool side, uint8 data) -> uint8;
  auto r4017(bool side, uint8 data) -> uint8;
  auto r4020(bool side, uint8 data) -> uint8;
  auto w4016(bool side, uint8 data) -> void;
  auto w4020(bool side, uint8 data) -> void;
};

extern VSArcadeBoard vsarcadeboard;
