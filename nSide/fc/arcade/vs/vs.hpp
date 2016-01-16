struct VSArcadeBoard {
  uint8 ram[2048];

  bool swap_controllers;
  uint16 dip; // 16-bit for DualSystem games

  auto init() -> void;
  auto load() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  auto set_dip(uint16 dip) -> void;
  auto read(uint16 addr) -> uint8;
  auto write(uint16 addr, uint8 data) -> void;

  auto serialize(serializer& s) -> void;

private:
  auto r4016() -> uint8;
  auto r4017() -> uint8;
  auto r4020() -> uint8;
  auto w4016(uint8 data) -> void;
  auto w4020(uint8 data) -> void;
};

extern VSArcadeBoard vsarcadeboard;
