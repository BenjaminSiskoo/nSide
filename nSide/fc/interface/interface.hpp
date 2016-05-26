namespace Famicom {

struct ID {
  enum : uint {
    //cartridges (folders)
    System,
    Famicom,
    VSSystem,
    PlayChoice10,
    FamicomBox,

    //memory (files)
    SystemManifest,
    PC10BIOS,
    PC10CharacterROM,
    PC10PaletteROM,
    FamicomBoxPRG,
    FamicomBoxCHR,

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
    ExpansionPort   = 4,
    ArcadePanel     = 8,
  };
};

struct Interface : Emulator::Interface {
  Interface();

  auto manifest() -> string;
  auto title() -> string;
  auto videoFrequency() -> double;
  auto videoColors() -> uint32;
  auto videoColor(uint32 color) -> uint64;
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

  auto cap(const string& name) -> bool override;
  auto get(const string& name) -> any override;
  auto set(const string& name, const any& value) -> bool override;

  //debugger functions
  auto exportMemory() -> void;

  struct DeviceRef {
    uint id;
    uint port[4];
  };
  vector<DeviceRef> deviceRefs;

private:
  auto addDevice(Device device) -> void;
};

struct Settings {
  bool colorEmulation = true;
  bool scanlineEmulation = true;

  uint controllerPort1 = 0;
  uint controllerPort2 = 0;
  uint expansionPort = 0;
  uint arcadePanel = 0;
};

extern Interface* interface;
extern Settings settings;

}
