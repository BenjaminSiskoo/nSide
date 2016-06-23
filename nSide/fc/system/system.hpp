struct Interface;

#include "peripherals.hpp"

struct System {
  enum class Region : uint { NTSC = 0, PAL = 1, Dendy = 2 };

  enum class Revision : uint {
    Famicom,
    VSSystem,
    PlayChoice10,
    FamicomBox,
  };

  inline auto loaded() const -> bool { return _loaded; }
  inline auto revision() const -> Revision { return _revision; }
  inline auto region() const -> Region { return _region; }
  inline auto cpuFrequency() const -> uint { return _cpuFrequency; }

  inline auto fc()   const { return _revision == Revision::Famicom; }
  inline auto vs()   const { return _revision == Revision::VSSystem; }
  inline auto pc10() const { return _revision == Revision::PlayChoice10; }
  inline auto fcb()  const { return _revision == Revision::FamicomBox; }

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
  serializer serialize();
  bool unserialize(serializer&);

  struct Information {
    string manifest;
  } information;

private:
  void serialize(serializer&);
  void serializeAll(serializer&);
  void serializeInit();

  bool _loaded = false;
  Revision _revision = Revision::Famicom;
  Region _region = Region::NTSC;
  uint _cpuFrequency = 0;
  uint _serializeSize = 0;

  friend class Cartridge;
  friend class Device;
};

extern System system;
