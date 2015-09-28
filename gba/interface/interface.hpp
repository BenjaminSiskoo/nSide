#ifndef GBA_HPP
namespace GameBoyAdvance {
#endif

struct ID {
  enum : unsigned {
    System,
    GameBoyAdvance,
  };

  enum : unsigned {
    SystemManifest,
    BIOS,

    Manifest,
    ROM,
    RAM,
    EEPROM,
    FlashROM,
  };

  enum : unsigned {
    Device = 1,
  };
};

struct Interface : Emulator::Interface {
  auto title() -> string;
  auto videoFrequency() -> double;
  auto audioFrequency() -> double;

  auto loaded() -> bool;
  auto sha256() -> string;
  auto group(unsigned id) -> unsigned;
  auto load(unsigned id) -> void;
  auto save() -> void;
  auto load(unsigned id, const stream& stream) -> void;
  auto save(unsigned id, const stream& stream) -> void;
  auto unload() -> void;

  auto power() -> void;
  auto reset() -> void;
  auto run() -> void;

  serializer serialize();
  auto unserialize(serializer&) -> bool;

  auto cheatSet(const lstring&) -> void;

  auto paletteUpdate(PaletteMode mode) -> void;

  //debugger functions
  auto exportMemory() -> void;

  Interface();

private:
  vector<Device> device;
};

extern Interface* interface;

#ifndef GBA_HPP
}
#endif
