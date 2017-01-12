struct System {
  enum class Region : uint { NTSC = 0, PAL = 1 };

  auto loaded() const -> bool { return information.loaded; }
  auto model() const -> Model { return information.model; }
  auto region() const -> Region { return information.region; }
  auto colorburst() const -> double { return information.colorburst; }

  auto run() -> void;

  auto load(Emulator::Interface* interface, Model model) -> bool;
  auto save() -> void;
  auto unload() -> void;

  auto power() -> void;

private:
  Emulator::Interface* interface = nullptr;

  struct Information {
    bool loaded = false;
    Model model = Model::SG1000;
    Region region = Region::NTSC;
    string manifest;
    double colorburst = 0.0;
  } information;
};

struct Peripherals {
  auto unload() -> void;
  auto reset() -> void;
  auto connect(uint port, uint device) -> void;

  Controller* hardware = nullptr;
  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
};

extern System system;
extern Peripherals peripherals;
