struct SS88006 : Chip {

uint8 prg_bank[3];
uint8 chr_bank[8];
uint2 mirror;
bool ram_enable;
bool ram_write_enable;
uint16 irq_counter;
uint16 irq_latch;
bool irq_enable;
uint16 irq_mask;

void enter() {
  while(true) {
    if(scheduler.sync == Scheduler::SynchronizeMode::All) {
      scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
    }

    if(irq_enable) {
      irq_counter = (irq_counter & ~irq_mask) | ((irq_counter - 1) & irq_mask);
      if((irq_counter & irq_mask) == irq_mask) cpu.set_irq_line(1);
    }
    tick();
  }
}

unsigned prg_addr(unsigned addr) const {
  switch((addr >> 13) & 3) {
  case 0: return (prg_bank[0] << 13) | (addr & 0x1fff);
  case 1: return (prg_bank[1] << 13) | (addr & 0x1fff);
  case 2: return (prg_bank[2] << 13) | (addr & 0x1fff);
  case 3: return (0xff << 13) | (addr & 0x1fff);
  }
}

unsigned chr_addr(unsigned addr) const {
  return (chr_bank[addr >> 10] << 10) | (addr & 0x03ff);
}

unsigned ciram_addr(unsigned addr) const {
  switch(mirror) {
  case 0: return (addr & 0x03ff) | ((addr & 0x0800) >> 1);
  case 1: return (addr & 0x03ff) | ((addr & 0x0400) >> 0);
  case 2: return (addr & 0x03ff) | 0x0000;
  case 3: return (addr & 0x03ff) | 0x0400;
  }
}

uint8 ram_read(unsigned addr) {
  if(ram_enable) return board.read(board.prgram, addr & 0x1fff);
}

void ram_write(unsigned addr, uint8 data) {
  if(ram_enable && ram_write_enable) board.write(board.prgram, addr & 0x1fff, data);
}

void reg_write(unsigned addr, uint8 data) {
  data &= 0x0f;
  switch(addr & 0xf003) {
  case 0x8000: prg_bank[0] = (prg_bank[0] & 0xf0) | (data << 0); break;
  case 0x8001: prg_bank[0] = (prg_bank[0] & 0x0f) | (data << 4); break;
  case 0x8002: prg_bank[1] = (prg_bank[1] & 0xf0) | (data << 0); break;
  case 0x8003: prg_bank[1] = (prg_bank[1] & 0x0f) | (data << 4); break;
  case 0x9000: prg_bank[2] = (prg_bank[2] & 0xf0) | (data << 0); break;
  case 0x9001: prg_bank[2] = (prg_bank[2] & 0x0f) | (data << 4); break;
  case 0x9002:
    ram_enable = data & 0x01;
    ram_write_enable = data & 0x02;
    break;
  case 0xa000: chr_bank[0] = (chr_bank[0] & 0xf0) | (data << 0); break;
  case 0xa001: chr_bank[0] = (chr_bank[0] & 0x0f) | (data << 4); break;
  case 0xa002: chr_bank[1] = (chr_bank[1] & 0xf0) | (data << 0); break;
  case 0xa003: chr_bank[1] = (chr_bank[1] & 0x0f) | (data << 4); break;
  case 0xb000: chr_bank[2] = (chr_bank[2] & 0xf0) | (data << 0); break;
  case 0xb001: chr_bank[2] = (chr_bank[2] & 0x0f) | (data << 4); break;
  case 0xb002: chr_bank[3] = (chr_bank[3] & 0xf0) | (data << 0); break;
  case 0xb003: chr_bank[3] = (chr_bank[3] & 0x0f) | (data << 4); break;
  case 0xc000: chr_bank[4] = (chr_bank[4] & 0xf0) | (data << 0); break;
  case 0xc001: chr_bank[4] = (chr_bank[4] & 0x0f) | (data << 4); break;
  case 0xc002: chr_bank[5] = (chr_bank[5] & 0xf0) | (data << 0); break;
  case 0xc003: chr_bank[5] = (chr_bank[5] & 0x0f) | (data << 4); break;
  case 0xd000: chr_bank[6] = (chr_bank[6] & 0xf0) | (data << 0); break;
  case 0xd001: chr_bank[6] = (chr_bank[6] & 0x0f) | (data << 4); break;
  case 0xd002: chr_bank[7] = (chr_bank[7] & 0xf0) | (data << 0); break;
  case 0xd003: chr_bank[7] = (chr_bank[7] & 0x0f) | (data << 4); break;
  case 0xe000: irq_latch = (irq_latch & 0xfff0) | (data << 0x0); break;
  case 0xe001: irq_latch = (irq_latch & 0xff0f) | (data << 0x4); break;
  case 0xe002: irq_latch = (irq_latch & 0xf0ff) | (data << 0x8); break;
  case 0xe003: irq_latch = (irq_latch & 0x0fff) | (data << 0xc); break;
  case 0xf000:
    cpu.set_irq_line(0);
    irq_counter = irq_latch;
    break;
  case 0xf001:
    cpu.set_irq_line(0);
    irq_enable = data & 0x01;
    irq_mask = data & 0x08 ? 0x000f : (data & 0x04 ? 0x00ff : (data & 0x02 ? 0x0fff : 0xffff));
    break;
  case 0xf002: mirror = data & 0x03; break;
  case 0xf003: break; //TODO: μPD7756C ADPCM sound IC
  }
}

void power() {
}

void reset() {
  prg_bank[0] = 0;
  prg_bank[1] = 0;
  prg_bank[2] = 0;
  chr_bank[0] = 0;
  chr_bank[1] = 0;
  chr_bank[2] = 0;
  chr_bank[3] = 0;
  chr_bank[4] = 0;
  chr_bank[5] = 0;
  chr_bank[6] = 0;
  chr_bank[7] = 0;
  mirror = 0;
  ram_enable = 0;
  ram_write_enable = 0;
  irq_counter = 0;
  irq_latch = 0;
  irq_enable = false;
  irq_mask = 0xffff;
}

void serialize(serializer& s) {
  s.array(prg_bank);
  s.array(chr_bank);
  s.integer(mirror);
  s.integer(ram_enable);
  s.integer(ram_write_enable);
  s.integer(irq_counter);
  s.integer(irq_latch);
  s.integer(irq_enable);
  s.integer(irq_mask);
}

SS88006(Board& board, Markup::Node& cartridge) : Chip(board) {
}

};
