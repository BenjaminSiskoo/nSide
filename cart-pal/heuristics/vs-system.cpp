struct VSSystemCartridge {
  VSSystemCartridge(const uint8* data, uint size);

  string markup;

//private:
  enum class Region : uint {
    NTSC = 0,
    PAL = 1,
  };

  uint mapper;
  uint mirror;
  uint prgrom;
  uint prgram;
  uint chrrom;
  uint chrram;
  bool     battery;
  bool     region;
  // NES 2.0
  bool     nes2;
  uint submapper;
  uint ppu;
};

VSSystemCartridge::VSSystemCartridge(const uint8* data, uint size) {
  if(size < 16) return;
  if(data[0] != 'N') return;
  if(data[1] != 'E') return;
  if(data[2] != 'S') return;
  if(data[3] !=  26) return;

  mapper = data[6].bits(4,7) | data[7].bits(4,7) << 4;
  mirror = data[6].bit(0) | data[6].bit(3) << 1;
  prgrom = data[4] * 0x4000;
  chrrom = data[5] * 0x2000;
  prgram = 0u;
  chrram = chrrom == 0u ? 8192u : 0u;
  battery = data[6].bit(1);
  nes2    = (data[7] & 0x0c) == 0x08;
  if(!nes2) {
    // ignore the last 9 bytes of headers that have "DiskDude!" or other
    // messages written there
    if(data[12] == 0 && data[13] == 0 && data[14] == 0 && data[15] == 0) {
      prgram = data[8] * 0x2000;
      region = data[9].bit(0);
    } else {
      return;
    }
  } else {
    mapper |= data[8].bits(0,3) << 8;
    submapper = data[8].bits(4,7);
    prgrom += data[9].bits(0,3) * 0x400000;
    chrrom += data[9].bits(4,7) * 0x200000;
    prgram  = (data[10].bits(0,3) == 0 ? 0 : 64) << data[10].bits(0,3); // no battery
    prgram += (data[10].bits(4,7) == 0 ? 0 : 64) << data[10].bits(4,7); // battery
    chrram  = (data[11].bits(0,3) == 0 ? 0 : 64) << data[11].bits(0,3); // no battery
    chrram += (data[11].bits(4,7) == 0 ? 0 : 64) << data[11].bits(4,7); // battery
    region = data[12].bit(0);
    ppu = data[13].bits(0,3);
  }

  if(prgram >= 0x800) prgram -= 0x800; // VS. System built-in RAM

  markup.append("side\n");
  string ppu_revision = "";
  switch(ppu) {
  case  0: ppu_revision = "RP2C03B"; break;
  case  1: ppu_revision = "RP2C03G"; break;
  case  2: ppu_revision = "RP2C04-0001"; break;
  case  3: ppu_revision = "RP2C04-0002"; break;
  case  4: ppu_revision = "RP2C04-0003"; break;
  case  5: ppu_revision = "RP2C04-0004"; break;
  case  6: ppu_revision = "RC2C03B"; break;
  case  7: ppu_revision = "RC2C03C"; break;
  case  8: ppu_revision = "RC2C05-01"; break;
  case  9: ppu_revision = "RC2C05-02"; break;
  case 10: ppu_revision = "RC2C05-03"; break;
  case 11: ppu_revision = "RC2C05-04"; break;
  case 12: ppu_revision = "RC2C05-05"; break;
  }
  markup.append("  ppu revision=", ppu_revision, "\n");
  markup.append("  controller port=2 device=gamepad\n");
  markup.append("  controller port=1 device=gamepad\n");

  switch(mapper) {
  default: // case  99:
    break;

  case   1: // SxROM-like
    markup.append("  chip type=MMC1B2\n");
    break;

  case   2: // UxROM-like
    markup.append("  chip type=74HC32\n");
    break;

  case 206: // NAMCOT-34xx-like
    markup.append("  chip type=108\n");
    break;
  }

  markup.append("  prg\n");
  if(prgrom) markup.append("    rom name=program.rom size=0x", hex(prgrom), "\n");
  if(prgram) {
    if(battery) markup.append("    ram name=save.ram size=0x", hex(prgram), "\n");
    else        markup.append("    ram name=work.ram size=0x", hex(prgram), " volatile\n");
  }

  markup.append("  chr\n");
  if(chrrom) markup.append("    rom name=character.rom size=0x", hex(chrrom), "\n");
  if(chrram) markup.append("    ram name=character.ram size=0x", hex(chrram), " volatile\n");
}
