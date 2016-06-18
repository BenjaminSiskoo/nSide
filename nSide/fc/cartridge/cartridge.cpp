#include <fc/fc.hpp>

namespace Famicom {

#include "markup.cpp"
#include "chip/chip.cpp"
#include "board/board.cpp"
Cartridge cartridge;

auto Cartridge::manifest() -> string {
  string manifest = information.markup.cartridge;

  //if(information.markup.famicomDiskSystem) {
  //  manifest.append("\n[[Famicom Disk System]]\n\n");
  //  manifest.append(information.markup.famicomDiskSystem);
  //}

  return manifest;
}

auto Cartridge::title() -> string {
  string title = information.title.cartridge;

  //if(information.title.famicomDiskSystem) {
  //  title.append(" + ", information.title.famicomDiskSystem);
  //}

  return title;
}

auto Cartridge::Enter() -> void {
  while(true) scheduler.synchronize(), cartridge.main();
}

auto Cartridge::main() -> void {
  board->main();
}

auto Cartridge::load() -> void {
  _region = Region::NTSC;

  information.markup.cartridge         = "";
  //information.markup.famicomDiskSystem = "";

  information.title.cartridge         = "";
  //information.title.famicomDiskSystem = "";

  interface->loadRequest(ID::Manifest, "manifest.bml", true);
  parseMarkup(information.markup.cartridge);

  if(board == nullptr) return;

  Hash::SHA256 sha;
  sha.data(board->prgrom.data(), board->prgrom.size());
  sha.data(board->chrrom.data(), board->chrrom.size());
  sha.data(board->instrom.data(), board->instrom.size());
  sha.data(board->keyrom.data(), board->keyrom.size());
  _sha256 = sha.digest();
}

auto Cartridge::unload() -> void {
  board->prgrom.reset();
  board->chrrom.reset();
  board->instrom.reset();
  board->keyrom.reset();
  memory.reset();
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

auto Cartridge::serialize(serializer& s) -> void {
  Thread::serialize(s);
  return board->serialize(s);
}

}
