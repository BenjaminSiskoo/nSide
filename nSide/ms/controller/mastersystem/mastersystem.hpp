struct MasterSystemControls : Controller {
  enum : uint {
    Pause, Reset,
  };

  MasterSystemControls(uint port);

  auto readData() -> uint8 override;
};
