//BANDAI-74*161/02/74

struct Bandai74_161_02_74 : Board {

uint2 prg_bank;
bool chr_plane;
uint2 chr_bank;
uint16 chr_abus;

void enter() {
  while(true) {
    if(scheduler.sync == Scheduler::SynchronizeMode::All) {
      scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
    }

    chr_abus_test(ppu.status.chr_abus);
    tick();
  }
}

uint8 prg_read(unsigned addr) {
  if(addr & 0x8000) return read(prgrom, (prg_bank << 15) | (addr & 0x7fff));
  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if(addr & 0x8000) {
    data &= prg_read(addr);
    prg_bank = (data & 0x03) >> 0;
    chr_plane = (data & 0x04) >> 2;
  }
}

unsigned chr_addr(unsigned addr) {
  switch(addr & 0x1000) {
  case 0x0000: return (addr & 0x0fff) | (chr_plane << 14) | (chr_bank << 12);
  case 0x1000: return (addr & 0x0fff) | (chr_plane << 14);
  }
}

uint8 chr_read(unsigned addr) {
  chr_abus_test(addr);
  if(addr & 0x2000) return ppu.ciram_read(addr & 0x07ff);
  return Board::chr_read(chr_addr(addr));
}

void chr_write(unsigned addr, uint8 data) {
  chr_abus_test(addr);
  if(addr & 0x2000) return ppu.ciram_write(addr & 0x07ff, data);
  Board::chr_write(chr_addr(addr), data);
}

void chr_abus_test(unsigned addr) {
  if((addr & 0x3000) != (chr_abus & 0x3000) && (addr & 0x3000) == 0x2000) {
    // NOR logic reverses bank numbers
    chr_bank = (((addr & 0x0300) ^ 0x0300) >> 8);
  }
  chr_abus = addr & 0x3000;
}

void power() {
}

void reset() {
  prg_bank = 0;
  chr_plane = 0;
  chr_bank = 0;
  chr_abus = 0;
}

void serialize(serializer& s) {
  Board::serialize(s);
  s.integer(prg_bank);
  s.integer(chr_plane);
  s.integer(chr_bank);
  s.integer(chr_abus);
}

Bandai74_161_02_74(Markup::Node& cartridge) : Board(cartridge) {
}

};
