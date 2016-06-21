struct Port { enum : uint {
  Controller1,
  Controller2,
  Expansion,
};};

struct Peripheral {
  struct ControllerPort1 { enum : uint {
    None,
    Gamepad,
    Multitap,
    Mouse,
  };};
  struct ControllerPort2 { enum : uint {
    None,
    Gamepad,
    Multitap,
    Mouse,
    SuperScope,
    Justifier,
    Justifiers,
  };};
  struct ExpansionPort { enum : uint {
    None,
    Satellaview,
    SuperDisc,
    S21FX,
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
