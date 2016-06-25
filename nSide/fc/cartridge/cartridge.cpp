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

  switch(system.revision()) {
  case System::Revision::Famicom:
    if(auto pathID = interface->load(ID::Famicom, "Famicom", "fc", true)) {
      information.pathID = pathID();
    } else return false;
    break;
  case System::Revision::VSSystem:
    if(auto pathID = interface->load(ID::VSSystem, "VS. System", "vs", true)) {
      information.pathID = pathID();
    } else return false;
    break;
  case System::Revision::PlayChoice10:
    if(auto pathID = interface->load(ID::PlayChoice10, "PlayChoice-10", "pc10", true)) {
      information.pathID = pathID();
    } else return false;
    break;
  //case System::Revision::FamicomBox:
  //  if(auto pathID = interface->load(ID::FamicomBox, "FamicomBox", "fcb", true)) {
  //    information.pathID = pathID();
  //  } else return false;
  //  break;
  }

  if(auto fp = interface->open(pathID(), "manifest.bml", File::Read, File::Required)) {
    information.manifest.cartridge = fp->reads();
  } else return false;
  auto document = BML::unserialize(information.manifest.cartridge);
  loadCartridge(document);
  if(board == nullptr) return false;

  //Famicom Disk System
  if(false) {

  }

  //Famicom
  else {
    Hash::SHA256 sha;
    //hash each ROM image that exists; any with size() == 0 is ignored by sha256_chunk()
    sha.data(board->prgrom.data(), board->prgrom.size());
    sha.data(board->chrrom.data(), board->chrrom.size());
    sha.data(board->instrom.data(), board->instrom.size());
    sha.data(board->keyrom.data(), board->keyrom.size());
    //finalize hash
    information.sha256 = sha.digest();
  }

  board->prgrom.writeProtect(true);
  board->prgram.writeProtect(false);
  board->chrrom.writeProtect(true);
  board->chrram.writeProtect(false);
  if(board->chip) board->chip->ram.writeProtect(false);
  board->instrom.writeProtect(true);
  board->keyrom.writeProtect(true);
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
