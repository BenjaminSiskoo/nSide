struct SG1000Controls : Controller {
  enum : uint {
    Pause,
  };

  SG1000Controls(uint port);

  auto readData() -> uint8 override;
};
