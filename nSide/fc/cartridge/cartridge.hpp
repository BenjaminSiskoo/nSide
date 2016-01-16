#include "chip/chip.hpp"
#include "board/board.hpp"

struct Cartridge : Thread, property<Cartridge> {
  enum class Region : uint { NTSC, PAL, Dendy };

  Cartridge() = default;
  ~Cartridge() { unload(); }

  static void Enter();
  void enter();

  auto loaded() const -> bool { return _loaded; }
  auto sha256() const -> string { return _sha256; }
  auto region() const -> Region { return _region; }

  auto power() -> void;
  auto reset() -> void;

  auto manifest() -> string;
  auto title() -> string;

  auto load(System::Revision) -> void;
  auto unload() -> void;

  auto serialize(serializer&) -> void;

  struct Memory {
    uint id;
    string name;
  };
  vector<Memory> memory;

  struct Information {
    struct Markup {
      string cartridge;
      //string famicomDiskSystem;
    } markup;

    struct Title {
      string cartridge;
      //string famicomDiskSystem;
    } title;
  } information;

//privileged:
  Board *board;

  auto prg_read(uint addr) -> uint8;
  auto prg_write(uint addr, uint8 data) -> void;

  auto chr_read(uint addr) -> uint8;
  auto chr_write(uint addr, uint8 data) -> void;

  //scanline() is for debugging purposes only:
  //boards must detect scanline edges on their own
  auto scanline(uint y) -> void;

private:
  friend class Interface;

  //markup.cpp
  auto parseMarkup(const char*) -> void;
  auto parseMarkupMemory(MappedRAM&, Markup::Node, uint id, bool writable) -> void;

  auto parseMarkupCartridge(Markup::Node) -> void;

  bool _loaded = false;
  string _sha256;
  Region _region = Region::NTSC;
};

extern Cartridge cartridge;
