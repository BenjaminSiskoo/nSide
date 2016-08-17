struct System {
  enum class Region : uint { NTSC = 0, PAL = 1 };

  auto loaded() const -> bool { return information.loaded; }
  auto model() const -> Model { return information.model; }
  auto region() const -> Region { return information.region; }
  auto colorburst() const -> double { return information.colorburst; }

  auto run() -> void;

  auto init() -> void;
  auto load(Model model) -> bool;
  auto save() -> void;
  auto unload() -> void;

  auto power() -> void;
  auto reset() -> void;

  struct Information {
    string manifest;
    bool loaded = false;
    Model model = Model::SG1000;
    Region region = Region::NTSC;
    double colorburst = 0.0;
  } information;
};

struct Peripherals {
  auto unload() -> void;
  auto reset() -> void;
  auto connect(uint port, uint device) -> void;

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
};

extern System system;
extern Peripherals peripherals;
