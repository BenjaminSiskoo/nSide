struct N163 : Chip {
  N163(Board& board, Markup::Node& board_node) : Chip(board) {
    string type = board_node["chip/type"].text();

    if(type == "129") revision = Revision::N129;
    if(type == "163") revision = Revision::N163;
    if(type == "175") revision = Revision::N175;
    if(type == "340") revision = Revision::N340;
  }

  auto enter() -> void {
    while(true) {
      if(irq_enable && irq_counter != 0x7fff) {
        irq_counter++;
        if(irq_counter == 0x7fff) cpu.set_irq_line(1);
      }

      if(!audio_disable) {
        if(++audio_channel_timer == 15) {
          audio_channel_timer = 0;
        }
      }
      tick();
    }
  }

  auto prg_read(uint addr) -> uint8 {
    switch(addr & 0xf800) {
    case 0x4800:
      if(revision == Revision::N129 || revision == Revision::N163) {
        uint8 byte = ram.read(audio_address);
        if(audio_auto_increment) audio_address++;
        return byte;
      }
      break;
    case 0x5000: return irq_counter & 0xff;
    case 0x5800: return ((irq_counter >> 8) & 0x7f) | (irq_enable << 7);
    case 0x6000: case 0x6800: case 0x7000: case 0x7800:
      if(ram_enable) return board.read(board.prgram, addr & 0x1fff);
    case 0x8000: case 0x8800: case 0x9000: case 0x9800:
    case 0xa000: case 0xa800: case 0xb000: case 0xb800:
    case 0xc000: case 0xc800: case 0xd000: case 0xd800:
      return board.read(board.prgrom, (prg_bank[(addr & 0x6000) >> 13] << 13) | (addr & 0x1fff));
    case 0xe000: case 0xe800: case 0xf000: case 0xf800:
      return board.read(board.prgrom, (0x3f << 13) | (addr & 0x1fff));
    default: return cpu.mdr();
    }
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) {
      if(ram_enable) {
        board.write(board.prgram, addr & 0x1fff, data);
      }
    } else reg_write(addr, data);
  }

  auto reg_write(uint addr, uint8 data) -> void {
    switch(addr & 0xf800) {
    case 0x4800: // Audio
      if(revision == Revision::N129 || revision == Revision::N163) {
        ram.write(audio_address, data);
        if(audio_auto_increment) audio_address++;
      }
      break;
    case 0x5000: // IRQ Low
      if(revision == Revision::N129 || revision == Revision::N163) {
        irq_counter = (irq_counter & 0xff00) | data;
        cpu.set_irq_line(0);
      }
      break;
    case 0x5800: // IRQ High
      if(revision == Revision::N129 || revision == Revision::N163) {
        irq_counter = (irq_counter & 0x00ff) | ((data & 0x7f) << 8);
        irq_enable = data & 0x80;
        cpu.set_irq_line(0);
      }
      break;
    case 0x8000: case 0x8800: case 0x9000: case 0x9800: // CHR Select
    case 0xa000: case 0xa800: case 0xb000: case 0xb800:
      chr_bank[(addr & 0x7800) >> 11] = data;
      break;
    case 0xc000: // Nametable Select/RAM Enable
      if(revision == Revision::N175) {
        ram_enable = data & 0x01;
      }
    case 0xc800: case 0xd000: case 0xd800: // Nametable Select
      if(revision == Revision::N129 || revision == Revision::N163) {
        chr_bank[(addr & 0x7800) >> 11] = data;
      }
      break;
    case 0xe000: // PRG Select 8000
      prg_bank[0] = data & 0x3f;
      if(revision == Revision::N129 || revision == Revision::N163) {
        audio_disable = data & 0x40;
      } else if(revision == Revision::N340) {
        mirror = (data & 0xc0) >> 6;
      }
      break;
    case 0xe800: // PRG Select A000
      prg_bank[1] = data & 0x3f;
      if(revision == Revision::N129 || revision == Revision::N163) {
        chrram_disable = (data & 0xc0) >> 6;
      }
      break;
    case 0xf000: // PRG Select C000
      prg_bank[2] = data & 0x3f;
      break;
    case 0xf800: // Sound RAM Address/Write Protection for External RAM
      if(revision == Revision::N129 || revision == Revision::N163) {
        audio_address = data & 0x7f;
        audio_auto_increment = data & 0x80;
      }
      break;
    }
  }

  auto ciram_addr(uint addr) -> uint {
    switch(mirror) {
    case 0: return (addr & 0x03ff) | (0x0000);
    case 1: return (addr & 0x03ff) | ((addr & 0x400) >> 0);
    case 2: return (addr & 0x03ff) | ((addr & 0x800) >> 1);
    case 3: return (addr & 0x03ff) | (0x0400);
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(revision == Revision::N175 || revision == Revision::N340) {
        return ppu.ciram_read(ciram_addr(addr));
      }
      addr &= 0x2fff;
    }
    uint8 bank = chr_bank[(addr & 0x3c00) >> 10];
    if(bank >= 0xe0 && ((addr & 0x2000) | !(chrram_disable & (1 << ((addr & 0x1000) >> 12))))) {
      addr = (addr & 0x03ff) | (bank << 10);
      if(board.chrram.size()) return board.read(board.chrram, addr);
      else                    return ppu.ciram_read(addr);
    } else return board.read(board.chrrom, ((bank << 10) | (addr & 0x3ff)));
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(revision == Revision::N175 || revision == Revision::N340) {
        return ppu.ciram_write(ciram_addr(addr), data);
      }
      addr &= 0x2fff;
    }
    uint8 bank = chr_bank[(addr & 0x3c00) >> 10];
    if(bank >= 0xe0 && ((addr & 0x2000) | !(chrram_disable & (1 << ((addr & 0x1000) >> 12))))) {
      addr = (addr & 0x03ff) | (bank << 10);
      if(board.chrram.size()) return board.write(board.chrram, addr, data);
      else                    return ppu.ciram_write(addr, data);
    }
  }

  auto serialize(serializer& s) -> void {
    s.array(ram.data(), ram.size());

    s.array(prg_bank);
    s.array(chr_bank);
    if(revision == Revision::N129 || revision == Revision::N163) {
      s.integer(irq_counter);
      s.integer(irq_enable);
      s.integer(chrram_disable);
      s.integer(audio_disable);
      s.integer(audio_address);
      s.integer(audio_auto_increment);
    } else if(revision == Revision::N175) {
      s.integer(ram_enable);
    } else if(revision == Revision::N340) {
      s.integer(mirror);
    }
  }

  auto power() -> void {
  }

  auto reset() -> void {
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
    chr_bank[8] = 0;
    chr_bank[9] = 0;
    chr_bank[10] = 0;
    chr_bank[11] = 0;
    irq_counter = 0;
    irq_enable = false;
    chrram_disable = 0;
    audio_disable = true;
    audio_address = 0x00;
    audio_auto_increment = false;
    ram_enable = revision != Revision::N175;
    mirror = 0;
  }

  enum class Revision : uint {
    N129,
    N163,
    N175,
    N340,
  } revision;

  uint6 prg_bank[3];
  uint8 chr_bank[12];
  uint15 irq_counter;
  bool irq_enable;
  uint2 chrram_disable;
  bool audio_disable;
  uint7 audio_address;
  bool audio_auto_increment;
  uint3 audio_channel;
  uint4 audio_channel_timer;
  bool ram_enable;
  uint2 mirror;
};
