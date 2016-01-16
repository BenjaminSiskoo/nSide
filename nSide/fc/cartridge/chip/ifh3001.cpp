struct IFH3001 : Chip {
  IFH3001(Board& board, Markup::Node& board_node) : Chip(board) {
  }

  auto enter() -> void {
    while(true) {
      if(scheduler.sync == Scheduler::SynchronizeMode::All) {
        scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
      }

      if(irq_enable && irq_counter) {
        if(--irq_counter == 0) cpu.set_irq_line(1);
      }
      tick();
    }
  }

  auto prg_addr(uint addr) const -> uint {
    switch(addr & 0xe000) {
    case 0x8000: return (prg_bank[0] << 13) | (addr & 0x1fff);
    case 0xa000: return (prg_bank[1] << 13) | (addr & 0x1fff);
    case 0xc000: return (prg_bank[2] << 13) | (addr & 0x1fff);
    case 0xe000: return (0xff << 13) | (addr & 0x1fff);
    }
  }

  auto chr_addr(uint addr) const -> uint {
    return (chr_bank[addr >> 10] << 10) | (addr & 0x03ff);
  }

  auto ciram_addr(uint addr) const -> uint {
    switch(mirror) {
    case 0: return (addr & 0x03ff) | ((addr & 0x0400) >> 0);
    case 1: return (addr & 0x03ff) | ((addr & 0x0800) >> 1);
    }
  }

  auto reg_write(uint addr, uint8 data) -> void {
    switch(addr & 0xf007) {
    case 0x8000:
    case 0x8001:
    case 0x8002:
    case 0x8003:
    case 0x8004:
    case 0x8005:
    case 0x8006:
    case 0x8007: prg_bank[0] = data; break;
    case 0x9001: mirror = data & 0x80; break;
    case 0x9003:
      irq_enable = data & 0x80;
      cpu.set_irq_line(0);
      break;
    case 0x9004:
      irq_counter = irq_latch;
      cpu.set_irq_line(0);
      break;
    case 0x9005: irq_latch = (irq_latch & 0x00ff) | (data << 8); break;
    case 0x9006: irq_latch = (irq_latch & 0xff00) | (data << 0); break;
    case 0xa000:
    case 0xa001:
    case 0xa002:
    case 0xa003:
    case 0xa004:
    case 0xa005:
    case 0xa006:
    case 0xa007: prg_bank[1] = data; break;
    case 0xb000: chr_bank[0] = data; break;
    case 0xb001: chr_bank[1] = data; break;
    case 0xb002: chr_bank[2] = data; break;
    case 0xb003: chr_bank[3] = data; break;
    case 0xb004: chr_bank[4] = data; break;
    case 0xb005: chr_bank[5] = data; break;
    case 0xb006: chr_bank[6] = data; break;
    case 0xb007: chr_bank[7] = data; break;
    case 0xc000:
    case 0xc001:
    case 0xc002:
    case 0xc003:
    case 0xc004:
    case 0xc005:
    case 0xc006:
    case 0xc007: prg_bank[2] = data; break;
    }
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prg_bank[0] = 0x00;
    prg_bank[1] = 0x01;
    prg_bank[2] = 0xfe;
    chr_bank[0] = 0;
    chr_bank[1] = 0;
    chr_bank[2] = 0;
    chr_bank[3] = 0;
    chr_bank[4] = 0;
    chr_bank[5] = 0;
    chr_bank[6] = 0;
    chr_bank[7] = 0;
    mirror = 0;
    irq_counter = 0;
    irq_latch = 0;
    irq_enable = false;
  }

  auto serialize(serializer& s) -> void {
    s.array(prg_bank);
    s.array(chr_bank);
    s.integer(mirror);
    s.integer(irq_counter);
    s.integer(irq_latch);
    s.integer(irq_enable);
  }

  bool prg_mode;
  uint8 prg_bank[3];
  uint8 chr_bank[8];
  bool mirror;
  uint16 irq_counter;
  uint16 irq_latch;
  bool irq_enable;
};
