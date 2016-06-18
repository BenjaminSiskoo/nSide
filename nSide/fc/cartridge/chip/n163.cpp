struct N163 : Chip {
  N163(Board& board, Markup::Node& boardNode) : Chip(board) {
    string type = boardNode["chip/type"].text();

    if(type == "129") revision = Revision::N129;
    if(type == "163") revision = Revision::N163;
    if(type == "175") revision = Revision::N175;
    if(type == "340") revision = Revision::N340;
  }

  auto main() -> void {
    if(!audioDisable) {
      if(++audioChannelTimer == 15) {
        audioChannelTimer = 0;
      }
    }
    tick();
  }

  auto prgRead(uint addr) -> uint8 {
    switch(addr & 0xf800) {
    case 0x4800:
      if(revision == Revision::N129 || revision == Revision::N163) {
        uint8 byte = ram.read(audioAddress);
        if(audioAutoIncrement) audioAddress++;
        return byte;
      }
      break;
    case 0x5000: return irqCounter & 0xff;
    case 0x5800: return ((irqCounter >> 8) & 0x7f) | (irqEnable << 7);
    case 0x6000: case 0x6800: case 0x7000: case 0x7800:
      if(ramEnable) return board.read(board.prgram, addr & 0x1fff);
    case 0x8000: case 0x8800: case 0x9000: case 0x9800:
    case 0xa000: case 0xa800: case 0xb000: case 0xb800:
    case 0xc000: case 0xc800: case 0xd000: case 0xd800:
      return board.read(board.prgrom, (prgBank[(addr & 0x6000) >> 13] << 13) | (addr & 0x1fff));
    case 0xe000: case 0xe800: case 0xf000: case 0xf800:
      return board.read(board.prgrom, (0x3f << 13) | (addr & 0x1fff));
    default: return cpu.mdr();
    }
  }

  auto prgWrite(uint addr, uint8 data) -> void {
    if((addr & 0xe000) == 0x6000) {
      if(ramEnable) {
        board.write(board.prgram, addr & 0x1fff, data);
      }
    } else regWrite(addr, data);
  }

  auto regWrite(uint addr, uint8 data) -> void {
    switch(addr & 0xf800) {
    case 0x4800: // Audio
      if(revision == Revision::N129 || revision == Revision::N163) {
        ram.write(audioAddress, data);
        if(audioAutoIncrement) audioAddress++;
      }
      break;
    case 0x5000: // IRQ Low
      if(revision == Revision::N129 || revision == Revision::N163) {
        irqCounter = (irqCounter & 0xff00) | data;
        cpu.setIRQLine(0);
      }
      break;
    case 0x5800: // IRQ High
      if(revision == Revision::N129 || revision == Revision::N163) {
        irqCounter = (irqCounter & 0x00ff) | ((data & 0x7f) << 8);
        irqEnable = data & 0x80;
        cpu.setIRQLine(0);
      }
      break;
    case 0x8000: case 0x8800: case 0x9000: case 0x9800: // CHR Select
    case 0xa000: case 0xa800: case 0xb000: case 0xb800:
      chrBank[(addr & 0x7800) >> 11] = data;
      break;
    case 0xc000: // Nametable Select/RAM Enable
      if(revision == Revision::N175) {
        ramEnable = data & 0x01;
      }
    case 0xc800: case 0xd000: case 0xd800: // Nametable Select
      if(revision == Revision::N129 || revision == Revision::N163) {
        chrBank[(addr & 0x7800) >> 11] = data;
      }
      break;
    case 0xe000: // PRG Select 8000
      prgBank[0] = data & 0x3f;
      if(revision == Revision::N129 || revision == Revision::N163) {
        audioDisable = data & 0x40;
      } else if(revision == Revision::N340) {
        mirror = (data & 0xc0) >> 6;
      }
      break;
    case 0xe800: // PRG Select A000
      prgBank[1] = data & 0x3f;
      if(revision == Revision::N129 || revision == Revision::N163) {
        chrramDisable = (data & 0xc0) >> 6;
      }
      break;
    case 0xf000: // PRG Select C000
      prgBank[2] = data & 0x3f;
      break;
    case 0xf800: // Sound RAM Address/Write Protection for External RAM
      if(revision == Revision::N129 || revision == Revision::N163) {
        audioAddress = data & 0x7f;
        audioAutoIncrement = data & 0x80;
      }
      break;
    }
  }

  auto ciramAddress(uint addr) -> uint {
    switch(mirror) {
    case 0: return (addr & 0x03ff) | (0x0000);
    case 1: return (addr & 0x03ff) | ((addr & 0x400) >> 0);
    case 2: return (addr & 0x03ff) | ((addr & 0x800) >> 1);
    case 3: return (addr & 0x03ff) | (0x0400);
    }
  }

  auto chrRead(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(revision == Revision::N175 || revision == Revision::N340) {
        return ppu.ciramRead(ciramAddress(addr));
      }
      addr &= 0x2fff;
    }
    uint8 bank = chrBank[(addr & 0x3c00) >> 10];
    if(bank >= 0xe0 && ((addr & 0x2000) | !(chrramDisable & (1 << ((addr & 0x1000) >> 12))))) {
      addr = (addr & 0x03ff) | (bank << 10);
      if(board.chrram.size()) return board.read(board.chrram, addr);
      else                    return ppu.ciramRead(addr);
    } else return board.read(board.chrrom, ((bank << 10) | (addr & 0x3ff)));
  }

  auto chrWrite(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(revision == Revision::N175 || revision == Revision::N340) {
        return ppu.ciramWrite(ciramAddress(addr), data);
      }
      addr &= 0x2fff;
    }
    uint8 bank = chrBank[(addr & 0x3c00) >> 10];
    if(bank >= 0xe0 && ((addr & 0x2000) | !(chrramDisable & (1 << ((addr & 0x1000) >> 12))))) {
      addr = (addr & 0x03ff) | (bank << 10);
      if(board.chrram.size()) return board.write(board.chrram, addr, data);
      else                    return ppu.ciramWrite(addr, data);
    }
  }

  auto serialize(serializer& s) -> void {
    s.array(ram.data(), ram.size());

    s.array(prgBank);
    s.array(chrBank);
    if(revision == Revision::N129 || revision == Revision::N163) {
      s.integer(irqCounter);
      s.integer(irqEnable);
      s.integer(chrramDisable);
      s.integer(audioDisable);
      s.integer(audioAddress);
      s.integer(audioAutoIncrement);
    } else if(revision == Revision::N175) {
      s.integer(ramEnable);
    } else if(revision == Revision::N340) {
      s.integer(mirror);
    }
  }

  auto power() -> void {
  }

  auto reset() -> void {
    prgBank[0] = 0;
    prgBank[1] = 0;
    prgBank[2] = 0;
    chrBank[0] = 0;
    chrBank[1] = 0;
    chrBank[2] = 0;
    chrBank[3] = 0;
    chrBank[4] = 0;
    chrBank[5] = 0;
    chrBank[6] = 0;
    chrBank[7] = 0;
    chrBank[8] = 0;
    chrBank[9] = 0;
    chrBank[10] = 0;
    chrBank[11] = 0;
    irqCounter = 0;
    irqEnable = false;
    chrramDisable = 0;
    audioDisable = true;
    audioAddress = 0x00;
    audioAutoIncrement = false;
    ramEnable = revision != Revision::N175;
    mirror = 0;
  }

  enum class Revision : uint {
    N129,
    N163,
    N175,
    N340,
  } revision;

  uint6 prgBank[3];
  uint8 chrBank[12];
  uint15 irqCounter;
  bool irqEnable;
  uint2 chrramDisable;
  bool audioDisable;
  uint7 audioAddress;
  bool audioAutoIncrement;
  uint3 audioChannel;
  uint4 audioChannelTimer;
  bool ramEnable;
  uint2 mirror;
};
