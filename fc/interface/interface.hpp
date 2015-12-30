#ifndef FC_HPP
namespace Famicom {
#endif

struct ID {
  enum : uint {
    //cartridges (folders)
    System,
    Famicom,
    PlayChoice10,
    VSSystem,

    //memory (files)
    SystemManifest,
    PC10BIOS,
    PC10CharacterROM,
    PC10PaletteROM,

    Manifest,
    ProgramROM,
    ProgramRAM,
    CharacterROM,
    CharacterRAM,
    ChipRAM,

    InstructionROM,
    KeyROM,

    //device ports (bitmask)
    ControllerPort1 = 1,
    ControllerPort2 = 2,
    ExpansionPort = 4,
  };
};

struct Interface : Emulator::Interface {
  Interface();

  auto manifest() -> string;
  auto title() -> string;
  auto videoFrequency() -> double;
  auto audioFrequency() -> double;

  auto loaded() -> bool;
  auto sha256() -> string;
  auto group(uint id) -> uint;
  auto load(uint id) -> void;
  auto save() -> void;
  auto load(uint id, const stream& stream) -> void;
  auto save(uint id, const stream& stream) -> void;
  auto unload() -> void;

  auto connect(uint port, uint device) -> void;
  auto power() -> void;
  auto reset() -> void;
  auto run() -> void;

  auto serialize() -> serializer;
  auto unserialize(serializer&) -> bool;

  auto cheatSet(const lstring&) -> void;

  auto paletteUpdate(PaletteMode mode) -> void;

  //debugger functions
  auto exportMemory() -> void;

  struct DeviceRef {
    uint id;
    uint port[3];
  };
  vector<DeviceRef> device_ref;

private:
  auto addDevice(Device device) -> void;
};

extern Interface* interface;

#ifndef FC_HPP
}
#endif
