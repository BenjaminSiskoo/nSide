struct Interface;

struct System {
  enum class Region : uint { NTSC = 0, PAL = 1, Dendy = 2 };

  inline auto loaded() const -> bool { return information.loaded; }
  inline auto model() const -> Model { return information.model; }
  inline auto region() const -> Region { return information.region; }
  inline auto colorburst() const -> double { return information.colorburst; }

  inline auto fc()   const { return information.model == Model::Famicom; }
  inline auto vs()   const { return information.model == Model::VSSystem; }
  inline auto pc10() const { return information.model == Model::PlayChoice10; }
  inline auto fcb()  const { return information.model == Model::FamicomBox; }

  auto run() -> void;
  auto runToSave() -> void;

  auto init() -> void;
  auto term() -> void;
  auto load(Model) -> bool;
  auto save() -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  //video.cpp
  auto configureVideoPalette() -> void;
  auto configureVideoEffects() -> void;

  //serialization.cpp
  auto serialize() -> serializer;
  auto unserialize(serializer&) -> bool;

private:
  struct Information {
    string manifest;
    bool loaded = false;
    Model model = Model::Famicom;
    Region region = Region::NTSC;
    double colorburst = 0.0;
  } information;

  uint serializeSize = 0;

  auto serialize(serializer&) -> void;
  auto serializeAll(serializer&) -> void;
  auto serializeInit() -> void;

  friend class Cartridge;
};

struct Peripherals {
  auto unload() -> void;
  auto reset() -> void;
  auto connect(uint port, uint device) -> void;

  Controller* controllerPort1 = nullptr;
  Controller* controllerPort2 = nullptr;
  Expansion* expansionPort = nullptr;
};

extern System system;
extern Peripherals peripherals;
