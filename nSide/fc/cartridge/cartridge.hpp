#include "chip/chip.hpp"
#include "board/board.hpp"

struct Cartridge : Thread {
  enum class Region : uint { NTSC, PAL, Dendy };

  static void Enter();
  void main();

  auto pathID() const -> uint { return information.pathID; }
  auto sha256() const -> string { return information.sha256; }
  auto region() const -> Region { return information.region; }
  auto manifest() const -> string;
  auto title() const -> string;

  auto power() -> void;
  auto reset() -> void;

  auto load() -> bool;
  auto save() -> void;
  auto unload() -> void;

  auto serialize(serializer&) -> void;

  struct Information {
    uint pathID = 0;
    string sha256;
    Region region = Region::NTSC;

    struct Manifest {
      string cartridge;
      string famicomDiskSystem;
    } manifest;

    struct Title {
      string cartridge;
      string famicomDiskSystem;
    } title;
  } information;

//privileged:
  Board* board;

  auto prgRead(uint addr) -> uint8;
  auto prgWrite(uint addr, uint8 data) -> void;

  auto chrRead(uint addr) -> uint8;
  auto chrWrite(uint addr, uint8 data) -> void;

  //scanline() is for debugging purposes only:
  //boards must detect scanline edges on their own
  auto scanline(uint y) -> void;

private:
  //load.cpp
  auto loadCartridge(Markup::Node) -> void;
  auto setupVS(Markup::Node&, Markup::Node&) -> void;

  auto loadMemory(MappedRAM&, Markup::Node, bool required, maybe<uint> id = nothing) -> void;

  //save.cpp
  auto saveCartridge(Markup::Node) -> void;

  auto saveMemory(MappedRAM&, Markup::Node, maybe<uint> = nothing) -> void;

  friend class Interface;
};

extern Cartridge cartridge;
