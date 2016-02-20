struct TC : Chip {
  TC(Board& board, Markup::Node& board_node) : Chip(board) {
    auto chip = board_node.find("chip");
    string type = chip(0)["type"].text();
    if(type == "TC0190FMC") revision = Revision::TC0190FMC;
    if(type == "TC0350FMR") revision = Revision::TC0350FMR;
    if(type == "TC0690FMR") revision = Revision::TC0690FMR;
    settings.pal16r4 = chip(1)["type"].text() == "PAL16R4";
  }

  auto enter() -> void {
    while(true) {
      if(scheduler.sync == Scheduler::SynchronizeMode::All) {
        scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
      }

      if(revision != Revision::TC0190FMC) {
        if(irq_delay) irq_delay--;
        irq_test(ppu.status.chr_abus);
        cpu.set_irq_line(irq_line);
      }
      tick();
    }
  }

  auto irq_test(uint addr) -> void {
    bool edge;
    // IRQs occur a little after they would on the MMC3.
    // Do they use the MC-ACC's behavior instead?
    edge = (chr_abus & 0x1000) && !(addr & 0x1000);
    if(edge) {
      if(irq_delay == 0) {
        if(irq_counter == 0xff) {
          irq_counter = irq_latch;
        } else if(++irq_counter == 0xff) {
          if(irq_enable) irq_line = 1;
        }
      }
      irq_delay = 6;
    }
    chr_abus = addr;
  }

  auto prg_addr(uint addr) const -> uint {
    switch((addr >> 13) & 3) {
    case 0: return (prg_bank[0] << 13) | (addr & 0x1fff);
    case 1: return (prg_bank[1] << 13) | (addr & 0x1fff);
    case 2: return (0x3e << 13) | (addr & 0x1fff);
    case 3: return (0x3f << 13) | (addr & 0x1fff);
    }
  }

  auto chr_addr(uint addr) const -> uint {
    if(addr <= 0x07ff) return (chr_bank[0] << 11) | (addr & 0x07ff);
    if(addr <= 0x0fff) return (chr_bank[1] << 11) | (addr & 0x07ff);
    if(addr <= 0x13ff) return (chr_bank[2] << 10) | (addr & 0x03ff);
    if(addr <= 0x17ff) return (chr_bank[3] << 10) | (addr & 0x03ff);
    if(addr <= 0x1bff) return (chr_bank[4] << 10) | (addr & 0x03ff);
    if(addr <= 0x1fff) return (chr_bank[5] << 10) | (addr & 0x03ff);
  }

  auto ciram_addr(uint addr) const -> uint {
    if(mirror == 0) return ((addr & 0x0400) >> 0) | (addr & 0x03ff);
    if(mirror == 1) return ((addr & 0x0800) >> 1) | (addr & 0x03ff);
  }

  auto reg_write(uint addr, uint8 data) -> void {
    switch(addr & 0xe003) {
    case 0x8000:
      prg_bank[0] = data & 0x3f;
      if(revision != Revision::TC0690FMR && !settings.pal16r4) mirror = data & 0x40;
      break;
    case 0x8001:
      prg_bank[1] = data & 0x3f;
      break;

    case 0x8002: chr_bank[0] = data; break;
    case 0x8003: chr_bank[1] = data; break;
    case 0xa000: chr_bank[2] = data; break;
    case 0xa001: chr_bank[3] = data; break;
    case 0xa002: chr_bank[4] = data; break;
    case 0xa003: chr_bank[5] = data; break;

    case 0xc000:
      if(revision != Revision::TC0190FMC) break;
      irq_latch = data;
      break;

    case 0xc001:
      if(revision != Revision::TC0190FMC) break;
      irq_counter = 0xff;
      break;

    case 0xc002:
      if(revision != Revision::TC0190FMC) break;
      irq_enable = true;
      break;

    case 0xc003:
      if(revision != Revision::TC0190FMC) break;
      irq_enable = false;
      irq_line = 0;
      break;

    case 0xe000:
      if(revision == Revision::TC0690FMR || settings.pal16r4) mirror = data & 0x40;
      break;
    }
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank[0] = 0;
    prg_bank[1] = 0;
    chr_bank[0] = 0;
    chr_bank[1] = 0;
    chr_bank[2] = 0;
    chr_bank[3] = 0;
    chr_bank[4] = 0;
    chr_bank[5] = 0;
    mirror = 0;
    irq_latch = 0;
    irq_counter = 0;
    irq_enable = false;
    irq_delay = 0;
    irq_line = 0;

    chr_abus = 0;
  }

  auto serialize(serializer& s) -> void {
    s.array(prg_bank);
    s.array(chr_bank);
    s.integer(mirror);
    s.integer(irq_latch);
    s.integer(irq_counter);
    s.integer(irq_enable);
    s.integer(irq_delay);
    s.integer(irq_line);

    s.integer(chr_abus);
  }

  enum class Revision : uint {
    TC0190FMC, // No IRQ
    TC0350FMR, // identical to TC0190 except with IRQ that no game uses
    TC0690FMR,
  } revision;

  struct Settings {
    bool pal16r4;
  } settings;

  uint8 prg_bank[2];
  uint8 chr_bank[6];
  bool mirror;
  uint8 irq_latch;
  uint8 irq_counter;
  bool irq_enable;
  uint irq_delay;
  bool irq_line;

  uint16 chr_abus;
};
