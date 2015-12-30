struct Interface;

#include "video.hpp"
#include "device.hpp"

struct System : property<System> {
  enum class Region : uint { NTSC = 0, PAL = 1, Autodetect = 2 };

  enum class Revision : uint {
    Famicom,
    PlayChoice10,
    VSSystem,
  } revision;

  System();

  auto run() -> void;
  auto runToSave() -> void;

  auto init() -> void;
  auto term() -> void;
  auto load(Revision) -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  void frame();
  void scanline();

  inline bool fc()   const { return revision == Revision::Famicom; }
  inline bool vs()   const { return revision == Revision::VSSystem; }
  inline bool pc10() const { return revision == Revision::PlayChoice10; }

  //return *active* system information (settings are cached upon power-on)
  readonly<Region> region;

  readonly<uint> cpuFrequency;
  readonly<uint> serializeSize;

  serializer serialize();
  bool unserialize(serializer&);

  struct Information {
    string manifest;
  } information;

private:
  void runThreadToSave();

  void serialize(serializer&);
  void serializeAll(serializer&);
  void serializeInit();

  friend class Cartridge;
  friend class Video;
  friend class Device;
};

extern System system;

#include <fc/scheduler/scheduler.hpp>

struct Configuration {
  Device::ID controllerPort1 = Device::ID::None;
  Device::ID controllerPort2 = Device::ID::None;
  Device::ID expansionPort = Device::ID::None;
  System::Region region = System::Region::Autodetect;
  bool random = true;
};

extern Configuration configuration;
