struct Interface;

#include "device.hpp"

struct System {
  enum class Region : uint { NTSC = 0, PAL = 1, Dendy = 2 };

  enum class Revision : uint {
    Famicom,
    PlayChoice10,
    VSSystem,
  } revision;

  auto region() const -> Region;
  auto cpuFrequency() const -> uint;

  auto run() -> void;
  auto runToSave() -> void;

  auto init() -> void;
  auto term() -> void;
  auto load(Revision) -> void;
  auto unload() -> void;
  auto power() -> void;
  auto reset() -> void;

  inline bool fc()   const { return revision == Revision::Famicom; }
  inline bool vs()   const { return revision == Revision::VSSystem; }
  inline bool pc10() const { return revision == Revision::PlayChoice10; }

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

  Region _region = Region::NTSC;
  uint _cpuFrequency = 0;
  uint _serializeSize = 0;

  friend class Cartridge;
  friend class Device;
};

extern System system;

#include <fc/scheduler/scheduler.hpp>
