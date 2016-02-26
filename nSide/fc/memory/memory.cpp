#include <fc/fc.hpp>

namespace Famicom {

Bus bus;

Bus::Bus() {
  lookup = new uint8 [64 * 1024];
  target = new uint32[64 * 1024];
}

Bus::~Bus() {
  delete[] lookup;
  delete[] target;
}

auto Bus::reset() -> void {
  function<auto (uint16, uint8) -> uint8> reader = [](uint16, uint8 data) { return data; };
  function<auto (uint16, uint8) -> void> writer = [](uint16, uint8) {};

  idcount = 0;
  map(reader, writer, 0x0000, 0xffff);
}

auto Bus::map(
  const function<uint8 (uint16, uint8)>& reader,
  const function<void (uint16, uint8)>& writer,
  uint16 addrlo, uint16 addrhi,
  uint size, uint base, uint mask
) -> void {
  assert(addrlo <= addrhi && addrlo <= 0xffff);
  assert(idcount < 255);

  uint id = idcount++;
  this->reader[id] = reader;
  this->writer[id] = writer;

  for(uint addr = addrlo; addr <= addrhi; addr++) {
    uint offset = reduce(addr, mask);
    if(size) offset = base + mirror(offset, size - base);
    lookup[addr] = id;
    target[addr] = offset;
  }
}

//$0000-07ff = RAM (2KB)
//$0800-1fff = RAM (mirror)
//$2000-2007 = PPU
//$2008-3fff = PPU (mirror)
//$4000-4017 = APU + I/O
//$4018-ffff = Cartridge

}
