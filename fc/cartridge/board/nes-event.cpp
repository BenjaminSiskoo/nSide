struct NES_Event : Board {

MMC1 mmc1;
uint2 prg_lock;
uint30 irq_counter;
uint4 dip;

void enter() {
  while(true) {
    if(scheduler.sync == Scheduler::SynchronizeMode::All) {
      scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
    }

    if(mmc1.writedelay) mmc1.writedelay--;
    switch(prg_lock) {
    case 0: if((mmc1.chr_bank[0] & 0x10) == 0x10) prg_lock++; break;
    case 1: if((mmc1.chr_bank[0] & 0x10) == 0x00) prg_lock++; break;
    case 2:
      if((mmc1.chr_bank[0] & 0x10) == 0x00) irq_counter++;
      if((mmc1.chr_bank[0] & 0x10) == 0x10) {
        irq_counter = 0x00000000;
        cpu.set_irq_line(0);
      }
      break;
    }
    if(irq_counter == (0x20000000 | (dip << 25))) cpu.set_irq_line(1);
    tick();
  }
}

unsigned prg_addr(unsigned addr) {
  if(prg_lock < 2) return addr & 0x7fff;
  switch(mmc1.chr_bank[0] & 0x08) {
  case 0: return ((mmc1.chr_bank[0] & 0x06) << 14) | (addr & 0x7fff);
  case 8: return mmc1.prg_addr(addr) | 0x20000;
  }
}

uint8 prg_read(unsigned addr) {
  if((addr & 0xe000) == 0x6000) {
    if(mmc1.ram_disable) return 0x00;
    if(prgram.size() > 0) return read(prgram, addr);
  }

  if(addr & 0x8000) {
    return read(prgrom, prg_addr(addr));
  }

  return cpu.mdr();
}

void prg_write(unsigned addr, uint8 data) {
  if((addr & 0xe000) == 0x6000) {
    if(mmc1.ram_disable) return;
    if(prgram.size() > 0) return write(prgram, addr, data);
  }

  if(addr & 0x8000) return mmc1.mmio_write(addr, data);
}

uint8 chr_read(unsigned addr) {
  if(addr & 0x2000) return ppu.ciram_read(mmc1.ciram_addr(addr));
  return Board::chr_read(addr);
}

void chr_write(unsigned addr, uint8 data) {
  if(addr & 0x2000) return ppu.ciram_write(mmc1.ciram_addr(addr), data);
  return Board::chr_write(addr, data);
}

void power() {
  mmc1.power();
}

void reset() {
  mmc1.reset();
  prg_lock = 0;
  irq_counter = 0x00000000;
}

void serialize(serializer& s) {
  Board::serialize(s);
  mmc1.serialize(s);
  s.integer(prg_lock);
  s.integer(irq_counter);
}

NES_Event(Markup::Node& cartridge) : Board(cartridge), mmc1(*this, cartridge) {
  dip = interface->dipSettings(Markup::Document({
    "setting name=Time\n",
    "  option value=4 name=6:14\n",
    "  option value=0 name=4:59\n",
    "  option value=1 name=5:18\n",
    "  option value=2 name=5:37\n",
    "  option value=3 name=5:56\n",
    "  option value=5 name=6:33\n",
    "  option value=6 name=6:52\n",
    "  option value=7 name=7:11\n",
  }));
}

};
