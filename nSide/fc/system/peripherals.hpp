struct Port { enum : uint {
  Controller1,
  Controller2,
  Expansion,
  Arcade,
};};

struct Device { enum : uint {
  None,

  //licensed peripherals
  Gamepad,
  FourPlayers,
  FourScore,
  BeamGun,
  FamilyTrainer,
  Vaus,

  //unlicensed peripherals
  SFCGamepad,
  Mouse,

  //arcade panels
  VSPanel,
};};

struct Peripherals {
  auto unload() -> void;
  auto reset() -> void;
  auto connect(uint port, uint id) -> void;

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
  Controller* expansionPort = nullptr;
};

extern Peripherals peripherals;
