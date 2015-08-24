#include <fc/fc.hpp>

#define MEMORY_CPP
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
  function<uint8 (unsigned)> reader = [](unsigned) { return cpu.mdr(); };
  function<void (unsigned, uint8)> writer = [](unsigned, uint8) {};

  idcount = 0;
  map(reader, writer, 0x0000, 0xffff);
}

auto Bus::map(
  const function<uint8 (unsigned)>& reader,
  const function<void (unsigned, uint8)>& writer,
  unsigned addrlo, unsigned addrhi,
  unsigned size, unsigned base, unsigned mask
) -> void {
  assert(addrlo <= addrhi && addrlo <= 0xffff);
  assert(idcount < 255);

  unsigned id = idcount++;
  this->reader[id] = reader;
  this->writer[id] = writer;

  for(unsigned addr = addrlo; addr <= addrhi; addr++) {
    unsigned offset = reduce(addr, mask);
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
