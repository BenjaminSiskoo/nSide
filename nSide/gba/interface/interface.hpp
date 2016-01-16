#ifndef GBA_HPP
namespace GameBoyAdvance {
#endif

struct ID {
  enum : uint {
    System,
    GameBoyAdvance,
  };

  enum : uint {
    SystemManifest,
    BIOS,

    Manifest,
    MROM,
    SRAM,
    EEPROM,
    FLASH,
  };

  enum : uint {
    Device = 1,
  };
};

struct Interface : Emulator::Interface {
  Interface();

  auto manifest() -> string;
  auto title() -> string;
  auto videoFrequency() -> double;
  auto audioFrequency() -> double;

  auto loaded() -> bool;
  auto group(uint id) -> uint;
  auto load(uint id) -> void;
  auto save() -> void;
  auto load(uint id, const stream& stream) -> void;
  auto save(uint id, const stream& stream) -> void;
  auto unload() -> void;

  auto power() -> void;
  auto reset() -> void;
  auto run() -> void;

  auto serialize() -> serializer;
  auto unserialize(serializer&) -> bool;

  auto cheatSet(const lstring&) -> void;

  auto paletteUpdate(PaletteMode mode) -> void;

  //debugger functions
  auto exportMemory() -> void;

private:
  vector<Device> device;
};

extern Interface* interface;

#ifndef GBA_HPP
}
#endif
