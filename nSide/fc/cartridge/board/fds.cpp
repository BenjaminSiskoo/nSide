struct FDS : Board {
  FDS(Markup::Node& board_node) : Board(board_node) {
  }

  auto enter() -> void {
    while(true) {
      if(scheduler.sync == Scheduler::SynchronizeMode::All) {
        scheduler.exit(Scheduler::ExitReason::SynchronizeEvent);
      }

      if(irq_enable && irq_counter > 0 && --irq_counter == 0) {
        cpu.set_irq_line(1);
        if(irq_repeat) irq_counter = irq_latch;
        else           irq_enable = false;
        irq_latch = 0; // for Kaettekita Mario Bros.?
      } else {
        //TODO: Drive timer emulation
      }
      tick();
    }
  }

  auto prg_read(uint addr) -> uint8 {
    switch(addr) {
    case 0x4030:
      {
        uint8 data = (irq_pending | (byte_transferred << 1));
        irq_pending = false;
        byte_transferred = false;
        // 0x10: CRC 0=pass; 1=fail
        // 0x40: End of disk head
        // 0x80: Disk read/write enable
        cpu.set_irq_line(0);
        return 0;
      }
    case 0x4031:
      return data_register;
    case 0x4032:
      return (
        (disk_slot_empty      << 0) |
        (disk_not_ready       << 1) |
        (disk_write_protected << 2)
      );
    case 0x4033:
      // 7-bit expansion input; 0x80 means low battery
      return 0x00;
    case 0x4090:
      return cpu.mdr() & 0xc0;
    case 0x4092:
      return cpu.mdr() & 0xc0;
    }
    if((addr & 0xffc0) == 0x4040) {
      return wavetable[addr & 0x3f] | (cpu.mdr() & 0xc0);
    }
    switch(addr & 0xe000) {
    case 0x6000:
    case 0x8000:
    case 0xa000:
    case 0xc000: return read(prgram, addr);
    case 0xe000: return read(prgrom, addr);
    }
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    switch(addr) {
    case 0x4020: irq_latch = (irq_latch & 0xff00) | (data << 0); break;
    case 0x4021: irq_latch = (irq_latch & 0x00ff) | (data << 8); break;
    case 0x4022:
      irq_repeat = data & 0x01;
      irq_enable = data & 0x02;
      irq_counter = irq_latch;
      byte_transferred = false;
      cpu.set_irq_line(0); // if pending IRQ flag is clear
      break;
    case 0x4023:
      disk_io_enable = data & 0x01;
      sound_io_enable = data & 0x02;
      break;
    case 0x4024:
      // clear pending IRQ flag
      if(!byte_transferred) cpu.set_irq_line(0);
      break;
    case 0x4025:
      fds_control = data & 0xf7;
      mirror = data & 0x08;
      break;
    case 0x4026:
      // 7-bit expansion output
      break;
    case 0x4080: break;
    case 0x4082: break;
    case 0x4083: break;
    case 0x4084: break;
    case 0x4085: break;
    case 0x4086: break;
    case 0x4087: break;
    case 0x4088: break;
    case 0x4089:
      wavetable_write_enable = data & 0x80;
      break;
    case 0x408a: break;
    }
    if((addr & 0xffc0) == 0x4040) {
      if(wavetable_write_enable) wavetable[addr & 0x3f] = data & 0x3f;
    }
    switch(addr & 0xe000) {
    case 0x6000:
    case 0x8000:
    case 0xa000:
    case 0xc000: write(prgram, addr, data); break;
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(mirror) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr);
    }
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(mirror) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr, data);
    }
    return Board::chr_write(addr, data);
  }

  auto power() -> void {
    reset();
  }

  auto reset() -> void {
    irq_counter = 0;
    irq_latch = 0;
    irq_repeat = false;
    irq_enable = false;

    disk_io_enable = false;
    sound_io_enable = false;

    fds_control = 0x26;
    mirror = false;

    irq_pending = false;
    byte_transferred = false;

    data_register = 0x00;

    disk_slot_empty = true;
    disk_not_ready = true;
    disk_write_protected = true;

    for(auto& n : wavetable) n = 0x00;
    wavetable_write_enable = false;
  }

  auto serialize(serializer& s) -> void {
    Board::serialize(s);

    s.integer(irq_counter);
    s.integer(irq_latch);
    s.integer(irq_repeat);
    s.integer(irq_enable);

    s.integer(disk_io_enable);
    s.integer(sound_io_enable);

    s.integer(fds_control);
    s.integer(mirror);

    s.integer(irq_pending);
    s.integer(byte_transferred);

    s.integer(data_register);

    s.integer(disk_slot_empty);
    s.integer(disk_not_ready);
    s.integer(disk_write_protected);

    s.array(wavetable);
    s.integer(wavetable_write_enable);
  }

  uint16 irq_counter;
  uint16 irq_latch;
  bool irq_repeat;
  bool irq_enable;

  bool disk_io_enable;
  bool sound_io_enable;

  uint8 fds_control;
  bool mirror;

  bool irq_pending;
  bool byte_transferred;

  uint8 data_register;

  bool disk_slot_empty;
  bool disk_not_ready;
  bool disk_write_protected;

  uint6 wavetable[0x40];
  bool wavetable_write_enable;

};
