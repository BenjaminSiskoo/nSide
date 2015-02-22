//Memory

unsigned Memory::size() const { return 0; }

//StaticRAM

uint8* StaticRAM::data() { return data_; }
unsigned StaticRAM::size() const { return size_; }

uint8 StaticRAM::read(unsigned addr) { return data_[addr]; }
void StaticRAM::write(unsigned addr, uint8 n) { data_[addr] = n; }
uint8& StaticRAM::operator[](unsigned addr) { return data_[addr]; }
const uint8& StaticRAM::operator[](unsigned addr) const { return data_[addr]; }

StaticRAM::StaticRAM(unsigned n) : size_(n) { data_ = new uint8[size_]; }
StaticRAM::~StaticRAM() { delete[] data_; }

//MappedRAM

void MappedRAM::reset() {
  if(data_) {
    delete[] data_;
    data_ = nullptr;
  }
  size_ = 0;
  write_protect_ = false;
}

void MappedRAM::map(uint8* source, unsigned length) {
  reset();
  data_ = source;
  size_ = data_ ? length : 0;
}

void MappedRAM::copy(const stream& memory) {
  if(data_) delete[] data_;
  //round size up to multiple of 256-bytes
  size_ = (memory.size() & ~255) + ((bool)(memory.size() & 255) << 8);
  data_ = new uint8[size_]();
  memory.read(data_, memory.size());
}

void MappedRAM::read(const stream& memory) {
  memory.read(data_, min(memory.size(), size_));
}

void MappedRAM::write_protect(bool status) { write_protect_ = status; }
uint8* MappedRAM::data() { return data_; }
unsigned MappedRAM::size() const { return size_; }

uint8 MappedRAM::read(unsigned addr) { return data_[addr]; }
void MappedRAM::write(unsigned addr, uint8 n) { if(!write_protect_) data_[addr] = n; }
const uint8& MappedRAM::operator[](unsigned addr) const { return data_[addr]; }
MappedRAM::MappedRAM() : data_(nullptr), size_(0), write_protect_(false) {}

//Bus

unsigned Bus::mirror(unsigned addr, unsigned size) {
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

unsigned Bus::reduce(unsigned addr, unsigned mask) {
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

uint8 Bus::read(unsigned addr) {
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

void Bus::write(unsigned addr, uint8 data) {
  cartridge.prg_write(addr, data);
  if(addr <= 0x1fff) return cpu.ram_write(addr, data);
  if(addr <= 0x3fff) return ppu.write(addr, data);
  if(addr <= 0x4017) return cpu.write(addr, data);
  if((addr & 0xe020) == 0x4020 && system.vs()) return cpu.write(addr, data);
}
