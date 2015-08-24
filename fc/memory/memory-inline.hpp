//Memory

auto Memory::size() const -> unsigned { return 0; }

//StaticRAM

auto StaticRAM::data() -> uint8* { return data_; }
auto StaticRAM::size() const -> unsigned { return size_; }

auto StaticRAM::read(unsigned addr) -> uint8 { return data_[addr]; }
auto StaticRAM::write(unsigned addr, uint8 n) -> void { data_[addr] = n; }
auto StaticRAM::operator[](unsigned addr) -> uint8& { return data_[addr]; }
auto StaticRAM::operator[](unsigned addr) const -> const uint8& { return data_[addr]; }

StaticRAM::StaticRAM(unsigned n) : size_(n) { data_ = new uint8[size_]; }
StaticRAM::~StaticRAM() { delete[] data_; }

//MappedRAM

auto MappedRAM::reset() -> void {
  if(data_) {
    delete[] data_;
    data_ = nullptr;
  }
  size_ = 0;
  write_protect_ = false;
}

auto MappedRAM::map(uint8* source, unsigned length) -> void {
  reset();
  data_ = source;
  size_ = data_ ? length : 0;
}

auto MappedRAM::copy(const stream& memory) -> void {
  if(data_) delete[] data_;
  //round size up to multiple of 256-bytes
  size_ = (memory.size() & ~255) + ((bool)(memory.size() & 255) << 8);
  data_ = new uint8[size_]();
  memory.read(data_, memory.size());
}

auto MappedRAM::read(const stream& memory) -> void {
  memory.read(data_, min(memory.size(), size_));
}

auto MappedRAM::write_protect(bool status) -> void { write_protect_ = status; }
auto MappedRAM::data() -> uint8* { return data_; }
auto MappedRAM::size() const -> unsigned { return size_; }

auto MappedRAM::read(unsigned addr) -> uint8 { return data_[addr]; }
auto MappedRAM::write(unsigned addr, uint8 n) -> void { if(!write_protect_) data_[addr] = n; }
auto MappedRAM::operator[](unsigned addr) const -> const uint8& { return data_[addr]; }

//Bus

auto Bus::mirror(unsigned addr, unsigned size) -> unsigned {
  if(size == 0) return 0;
  unsigned base = 0;
  unsigned mask = 1 << 15;
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

auto Bus::reduce(unsigned addr, unsigned mask) -> unsigned {
  while(mask) {
    unsigned bits = (mask & -mask) - 1;
    addr = ((addr >> 1) & ~bits) | (addr & bits);
    mask = (mask & (mask - 1)) >> 1;
  }
  return addr;
}

//$0000-07ff = RAM (2KB)
//$0800-1fff = RAM (mirror)
//$2000-2007 = PPU
//$2008-3fff = PPU (mirror)
//$4000-4017 = APU + I/O
//$4018-ffff = Cartridge

auto Bus::read(unsigned addr) -> uint8 {
  uint8 data = cartridge.prg_read(addr);
       if(addr <= 0x1fff) data = cpu.ram_read(addr);
  else if(addr <= 0x3fff) data = ppu.read(addr);
  else if(addr <= 0x4017) data = cpu.read(addr);
  else if((addr & 0xe020) == 0x4020 && system.vs()) data = cpu.read(addr);

  if(cheat.enable()) {
    if(auto result = cheat.find(addr, data)) return result();
  }

  return data;
}

auto Bus::write(unsigned addr, uint8 data) -> void {
  cartridge.prg_write(addr, data);
  if(addr <= 0x1fff) return cpu.ram_write(addr, data);
  if(addr <= 0x3fff) return ppu.write(addr, data);
  if(addr <= 0x4017) return cpu.write(addr, data);
  if((addr & 0xe020) == 0x4020 && system.vs()) return cpu.write(addr, data);
}
