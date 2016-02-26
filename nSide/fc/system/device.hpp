struct Device {
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
  // C++ prohibits counting the number of items in an enum. Take care
  // and update the number of unique devices in interface.cpp whenever a device
  // is added or removed.
  // Currently, there are 9 devices, counting None.

  Device();
  ~Device();

  void connect(uint port, Device::ID id);

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
  Controller* expansionPort = nullptr;
};

extern Device device;
