struct Port { enum : uint {
  Controller1,
  Controller2,
  Expansion,
  Arcade,
};};

struct Peripheral {
  struct ControllerPort1 { enum : uint {
    None,
    Gamepad,
    FourScore,
    //unlicensed
    SNESGamepad,
    Mouse,
  };};
  struct ControllerPort2 { enum : uint {
    None,
    Gamepad,
    FourScore,
    Zapper,
    PowerPad,
    Vaus,
    //unlicensed
    SNESGamepad,
    Mouse,
  };};
  struct ExpansionPort { enum : uint {
    None,
    Gamepad,
    FourPlayers,
    BeamGun,
    FamilyTrainer,
    Vaus,
    //unlicensed peripherals
    SFCGamepad,
    Mouse,
  };};
  struct ArcadePanel { enum : uint {
    None,
    VSPanel,
  };};
};

struct Peripherals {
  auto unload() -> void;
  auto reset() -> void;
  auto connect(uint port, uint index) -> void;

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
  Expansion* expansionPort = nullptr;
};

extern Peripherals peripherals;
