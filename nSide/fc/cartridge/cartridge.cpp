#include <fc/fc.hpp>

namespace Famicom {

#include "load.cpp"
#include "save.cpp"
#include "chip/chip.cpp"
#include "board/board.cpp"
#include "serialization.cpp"
Cartridge cartridge;

auto Cartridge::manifest() const -> string {
  string manifest = information.manifest.cartridge;
  if(information.manifest.famicomDiskSystem) manifest.append("\n[[Famicom Disk System]]\n\n", information.manifest.famicomDiskSystem);
  return manifest;
}

auto Cartridge::title() const -> string {
  string title = information.title.cartridge;
  if(information.title.famicomDiskSystem) title.append(" + ", information.title.famicomDiskSystem);
  return title;
}

auto Cartridge::Enter() -> void {
  while(true) scheduler.synchronize(), cartridge.main();
}

auto Cartridge::main() -> void {
  board->main();
}

auto Cartridge::load() -> bool {
  information = Information();
  _sha256 = "";
  _region = Region::NTSC;

  if(auto fp = interface->open((uint)system.revision() + 1, "manifest.bml", File::Read, File::Required)) {
    information.manifest.cartridge = fp->reads();
  } else return false;
  auto document = BML::unserialize(information.manifest.cartridge);
  loadCartridge(document);
  if(board == nullptr) return false;

  if(false/*Famicom Disk System*/) {

  } else {
    Hash::SHA256 sha;
    //hash each ROM image that exists; any with size() == 0 is ignored by sha256_chunk()
    sha.data(board->prgrom.data(), board->prgrom.size());
    sha.data(board->chrrom.data(), board->chrrom.size());
    sha.data(board->instrom.data(), board->instrom.size());
    sha.data(board->keyrom.data(), board->keyrom.size());
    //finalize hash
    _sha256 = sha.digest();
  }

  return true;
}

auto Cartridge::save() -> void {
  saveCartridge(BML::unserialize(information.manifest.cartridge));
}

auto Cartridge::unload() -> void {
  board->prgrom.reset();
  board->chrrom.reset();
  board->instrom.reset();
  board->keyrom.reset();
}

auto Cartridge::power() -> void {
  board->power();
}

auto Cartridge::reset() -> void {
  create(Cartridge::Enter, system.cpuFrequency());
  board->reset();
}

auto Cartridge::prgRead(uint addr) -> uint8 {
  return board->prgRead(addr);
}

auto Cartridge::prgWrite(uint addr, uint8 data) -> void {
  return board->prgWrite(addr, data);
}

auto Cartridge::chrRead(uint addr) -> uint8 {
  return board->chrRead(addr);
}

auto Cartridge::chrWrite(uint addr, uint8 data) -> void {
  return board->chrWrite(addr, data);
}

auto Cartridge::scanline(uint y) -> void {
  return board->scanline(y);
}

}
