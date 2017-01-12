#include <a2600/a2600.hpp>

namespace Atari2600 {

#include "serialization.cpp"
Cartridge cartridge;

auto Cartridge::load() -> bool {
  information = Information();

  if(auto pathID = platform->load(ID::Atari2600, "Atari 2600", "a26")) {
    information.pathID = pathID();
  } else return false;

  if(auto fp = platform->open(pathID(), "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  information.title = document["information/title"].text();
  if(document["board/region"].text() == "ntsc")  information.region = Region::NTSC;
  if(document["board/region"].text() == "pal")   information.region = Region::PAL;
  if(document["board/region"].text() == "secam") information.region = Region::SECAM;

  if(auto node = document["board/rom"]) {
    rom.size = node["size"].natural();
    rom.mask = bit::round(rom.size) - 1;
    if(rom.size) {
      rom.data = new uint8[rom.mask];
      if(auto name = node["name"].text()) {
        if(auto fp = platform->open(pathID(), name, File::Read, File::Required)) {
          fp->read(rom.data, rom.size);
        }
      }
    }
  }

  if(auto node = document["board/ram"]) {
    ram.size = node["size"].natural();
    ram.mask = bit::round(ram.size) - 1;
    if(ram.size) {
      ram.data = new uint8[ram.mask];
      if(auto name = node["name"].text()) {
        if(auto fp = platform->open(pathID(), name, File::Read)) {
          fp->read(ram.data, ram.size);
        }
      }
    }
  }

  information.sha256 = Hash::SHA256(rom.data, rom.size).digest();
  return true;
}

auto Cartridge::save() -> void {
  auto document = BML::unserialize(information.manifest);

  if(auto name = document["board/ram/name"].text()) {
    if(auto fp = platform->open(pathID(), name, File::Write)) {
      fp->write(ram.data, ram.size);
    }
  }
}

auto Cartridge::unload() -> void {
  delete[] rom.data;
  delete[] ram.data;
  rom = Memory();
  ram = Memory();
}

auto Cartridge::power() -> void {
}

auto Cartridge::reset() -> void {
}

auto Cartridge::access(uint13 addr, uint8 data) -> uint8 {
  if(!addr.bit(12)) return data;

  if(ram.size) {
         if((addr & rom.mask) < ram.size << 0) return ram.data[addr & ram.mask] = data;
    else if((addr & rom.mask) < ram.size << 1) return ram.data[addr & ram.mask];
  }

  return rom.data[addr & rom.mask];
}

}
