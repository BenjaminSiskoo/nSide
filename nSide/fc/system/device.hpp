struct Device {
  enum class Port : uint {
    Controller1,
    Controller2,
    Expansion,
    ArcadePanel,
  };

  enum class ID : uint {
    None,

    // licensed devices
    Gamepad,
    FourPlayers,
    FourScore,
    BeamGun,
    FamilyTrainer,

    // unlicensed devices
    SFCGamepad,
    Mouse,

    // arcade panels
    VSPanel,
  };

  Device();
  ~Device();

  void connect(uint port, Device::ID id);

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
  Controller* expansionPort = nullptr;
};

extern Device device;
