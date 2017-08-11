#include <md/md.hpp>

namespace MegaDrive {

Cartridge cartridge;
#include "serialization.cpp"

auto Cartridge::load() -> bool {
  information = {};

  if(auto loaded = platform->load(ID::MegaDrive, "Mega Drive", "md", {"Auto", "NTSC-J", "NTSC-U", "PAL"})) {
    information.pathID = loaded.pathID();
    information.region = loaded.option();
  } else return false;

  if(auto fp = platform->open(pathID(), "manifest.bml", File::Read, File::Required)) {
    information.manifest = fp->reads();
  } else return false;

  auto document = BML::unserialize(information.manifest);
  information.title = document["information/title"].text();

  if(!region() || region() == "Auto") {
    if(document["board/region"].text() == "ntsc-j") information.region = "NTSC-J";
    if(document["board/region"].text() == "ntsc-u") information.region = "NTSC-U";
    if(document["board/region"].text() == "pal") information.region = "PAL";
  }

  if(auto node = document["board/rom"]) {
    rom.size = node["size"].natural();
    auto addrRange = node["map/address"].text().split("-", 1L);
    rom.addrLo = addrRange(0) ? addrRange(0).hex() : 0x000000;
    rom.addrHi = addrRange(1) ? addrRange(1).hex() : 0x3fffff;
    rom.mask = 0;
    if(rom.size) {
      rom.data = new uint8[rom.size];
      if(auto name = node["name"].text()) {
        if(auto fp = platform->open(pathID(), name, File::Read, File::Required)) {
          fp->read(rom.data, rom.size);
        }
      }
    }
  }

  if(auto node = document["board/ram"]) {
    ram.size = node["size"].natural();
    auto addrRange = node["map/address"].text().split("-", 1L);
    ram.addrLo = addrRange(0) ? addrRange(0).hex() : 0x200000;
    ram.addrHi = addrRange(1) ? addrRange(1).hex() : 0x3fffff;
    ram.mask = node["map/mask"].natural();
    if(ram.size) {
      ram.data = new uint8[ram.size];
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

  if(document["board/ram/volatile"]) return;
  if(auto name = document["board/ram/name"].text()) {
    if(auto fp = platform->open(pathID(), name, File::Write)) {
      fp->write(ram.data, ram.size);
    }
  }
}

auto Cartridge::unload() -> void {
  delete[] rom.data;
  delete[] ram.data;
  rom = {};
  ram = {};
}

auto Cartridge::power() -> void {
  ramEnable = 1;
  ramWritable = 1;
  for(auto n : range(8)) bank[n] = n;
}

alwaysinline auto mirror(uint addr, uint size) -> uint {
  if(size == 0) return 0;
  uint base = 0;
  uint mask = 1 << 23;
  while(addr >= size) {
    while(!(addr & mask)) mask >>= 1;
    addr -= mask;
    if(size > mask) {
      size -= mask;
      base += mask;
    }
    mask >>= 1;
  }
  return base + addr;
}

auto Cartridge::read(uint24 addr) -> uint16 {
  uint16 data = 0x0000;
  if(addr >= rom.addrLo && addr <= rom.addrHi) {
    uint24 romAddr = mirror(bank[addr >> 19 & 7] << 19 | (addr & 0x7ffff), rom.size);
    data.byte(1) = rom.data[romAddr + 0];
    data.byte(0) = rom.data[romAddr + 1];
  }
  if(ram.size && addr >= (ram.addrLo & ~1) && addr <= (ram.addrHi | 1) && ramEnable) {
    if(ram.mask & 1) addr >>= 1;
    addr = mirror(addr, ram.size);
    if(ram.mask & 1) {
      data.byte(1 - ram.addrLo.bit(0)) = ram.data[addr];
    } else {
      data.byte(1) = ram.data[addr + 0];
      data.byte(0) = ram.data[addr + 1];
    }
  }
  return data;
}

auto Cartridge::write(uint24 addr, uint16 data) -> void {
  //emulating RAM write protect bit breaks some commercial software
  if(ram.size && addr >= (ram.addrLo & ~1) && addr <= (ram.addrHi | 1) && ramEnable /* && ramWritable */) {
    if(ram.mask & 1) addr >>= 1;
    addr = mirror(addr, ram.size);
    if(ram.mask & 1) {
      ram.data[addr] = data.byte(1 - ram.addrLo.bit(0));
    } else {
      ram.data[addr + 0] = data.byte(1);
      ram.data[addr + 1] = data.byte(0);
    }
  }
}

auto Cartridge::readIO(uint24 addr) -> uint16 {
  return 0x0000;
}

auto Cartridge::writeIO(uint24 addr, uint16 data) -> void {
  if(addr == 0xa130f1) ramEnable = data.bit(0), ramWritable = data.bit(1);
  if(addr == 0xa130f3) bank[1] = data;
  if(addr == 0xa130f5) bank[2] = data;
  if(addr == 0xa130f7) bank[3] = data;
  if(addr == 0xa130f9) bank[4] = data;
  if(addr == 0xa130fb) bank[5] = data;
  if(addr == 0xa130fd) bank[6] = data;
  if(addr == 0xa130ff) bank[7] = data;
}

}
