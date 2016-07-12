struct Interface;

struct System {
  enum class Region : uint { NTSC = 0, PAL = 1, Dendy = 2 };

  enum class Revision : uint {
    Famicom,
    VSSystem,
    PlayChoice10,
    FamicomBox,
  };

  inline auto loaded() const -> bool { return information.loaded; }
  inline auto revision() const -> Revision { return information.revision; }
  inline auto region() const -> Region { return information.region; }
  inline auto colorburst() const -> double { return information.colorburst; }

  inline auto fc()   const { return information.revision == Revision::Famicom; }
  inline auto vs()   const { return information.revision == Revision::VSSystem; }
  inline auto pc10() const { return information.revision == Revision::PlayChoice10; }
  inline auto fcb()  const { return information.revision == Revision::FamicomBox; }

  auto run() -> void;
  auto runToSave() -> void;

  auto init() -> void;
  auto term() -> void;
  auto load(Revision) -> bool;
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
    Revision revision = Revision::Famicom;
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
