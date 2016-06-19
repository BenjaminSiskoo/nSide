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
  FourScore13,
  FourScore24,
  Zapper,
  PowerPad,
  Vaus,

  GamepadE,
  FourPlayers,
  BeamGun,
  FamilyTrainer,
  VausE,

  //unlicensed peripherals
  SNESGamepad,
  Mouse,
  SFCGamepad,
  MouseE,

  //arcade panels
  VSPanel,
};};

struct Peripherals {
  auto unload() -> void;
  auto reset() -> void;
  auto connect(uint port, uint id) -> void;

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
  Expansion* expansionPort = nullptr;
};

extern Peripherals peripherals;
