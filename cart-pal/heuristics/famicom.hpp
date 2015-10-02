struct FamicomCartridge {
  FamicomCartridge(const uint8_t* data, unsigned size);
  auto vsSystemHeuristic(const uint8_t* data, unsigned size) -> void;

  string markup;

//private:
  unsigned mapper;
  unsigned mirror;
  unsigned prgrom;
  unsigned prgram;
  unsigned chrrom;
  unsigned chrram;
  bool     battery;
  bool     region;
  bool     vs;
  // NES 2.0
  bool     pc10;
  bool     nes2;
  unsigned submapper;
  unsigned ppu;
};

FamicomCartridge::FamicomCartridge(const uint8_t* data, unsigned size) {
  if(size < 16) return;
  if(data[0] != 'N') return;
  if(data[1] != 'E') return;
  if(data[2] != 'S') return;
  if(data[3] !=  26) return;

  mapper = ((data[7] >> 4) << 4) | (data[6] >> 4);
  mirror = ((data[6] & 0x08) >> 2) | (data[6] & 0x01);
  prgrom = data[4] * 0x4000;
  chrrom = data[5] * 0x2000;
  prgram = 0u;
  chrram = chrrom == 0u ? 8192u : 0u;
  battery = data[6] & 0x02;
  vs      = data[7] & 0x01;
  nes2    = (data[7] & 0x0c) == 0x08;
  if(!nes2) {
    // ignore the last 9 bytes of headers that have "DiskDude!" or other
    // messages written there
    if(data[12] == 0 && data[13] == 0 && data[14] == 0 && data[15] == 0) {
      prgram = data[8] * 0x2000;
      region = data[9] & 0x01;
    } else {
      mapper &= 0x0f;
      vs = false;
      pc10 = false;
    }
  } else {
    pc10 = data[7] & 0x02;
    mapper |= (data[8] & 0x0f) << 8;
    submapper |= data[8] >> 4;
    prgrom += (data[9] & 0x0f) * 0x400000;
    chrrom += (data[9] >> 4) * 0x200000;
    prgram = (data[10] & 0x0f == 0 ? 0 : 64) << (data[10] & 0x0f); // no battery
    prgram += (data[10] >> 4 == 0 ? 0 : 64) << (data[10] >> 4); // battery
    chrram = (data[11] & 0x0f == 0 ? 0 : 64) << (data[11] & 0x0f); // no battery
    chrram += (data[11] >> 4 == 0 ? 0 : 64) << (data[11] >> 4); // battery
    region = data[12] & 0x01;
    ppu = data[13] & 0x0f;
  }

  if(vs) { vsSystemHeuristic(data, size); return; }

  markup.append("cartridge region=", region == 0 ? "NTSC" : "PAL", "\n");

  switch(mapper) {
  default:
    if(prgram) {
      markup.append("  board type=HVC-FAMILYBASIC\n");
    } else if(prgrom <= 8192) {
      markup.append("  board type=NAMCOT-3301\n");
    } else if(prgrom <= 16384) {
      markup.append("  board type=NES-NROM-128\n");
    } else {
      markup.append("  board type=NES-NROM-256\n");
    }
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   1:
  case 155:
    if(prgram <= 8192) {
      markup.append("  board type=NES-SUROM\n");
    } else {
      markup.append("  board type=NES-SXROM\n");
    }
    markup.append("  chip type=MMC1", mapper != 155 ? "B2" : "A", "\n");
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case   2:
  case  94:
  case 180:
         if(mapper == 94)     markup.append("  board type=HVC-UN1ROM\n");
    else if(prgrom <= 131072) markup.append("  board type=NES-UNROM\n");
    else                      markup.append("  board type=NES-UOROM\n");
    markup.append("  chip type=74HC", mapper != 180 ? "32" : "08", "\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   3:
  case 185:
    markup.append("  board type=NES-CNROM\n");
    if(mapper == 185) {
      markup.append("    security pass=0x", hex(submapper & 3), "\n");
    }
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   4:
  case 118:
  case 119:
    if(prgram != 0x400) {
      switch(mapper) {
      case   4:
        if(mirror & 2) markup.append("  board type=NES-TR1ROM\n");
        else           markup.append("  board type=NES-TLROM\n");
        break;
      case 118: markup.append("  board type=NES-TLSROM\n"); break;
      case 119: markup.append("  board type=NES-TQROM\n"); break;
      }
      markup.append("  chip type=MMC3B\n");
    } else {
      markup.append("  board type=NES-HKROM\n");
      markup.append("  chip type=MMC6n");
    }
    if(!nes2 && !prgram) prgram = 8192;
    if(mapper == 119 && !nes2 && !chrram) chrram = 0x2000;
    break;

  case   5:
    markup.append("  board type=NES-ELROM\n");
    markup.append("  chip type=MMC5\n");
    if(!nes2 && !prgram) prgram = 32768;
    break;

  case   7:
    markup.append("  board type=NES-AOROM\n");
    break;

  case   9:
    markup.append("  board type=NES-PNROM\n");
    markup.append("  chip type=MMC2\n");
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  10:
    markup.append("  board type=HVC-FKROM\n");
    markup.append("  chip type=MMC4\n");
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  13:
    markup.append("  board type=NES-CPROM\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  16:
  case 153:
  case 159:
    switch(mapper) {
    case  16:
      markup.append("  board type=BANDAI-FCG\n");
      markup.append("  chip type=LZ93D50\n");
      markup.append("  chip type=24C02\n");
      break;
    case 153:
      markup.append("  board type=BANDAI-JUMP2\n");
      markup.append("  chip type=LZ93D50\n");
      if(!nes2 && !prgram) prgram = 8192;
      break;
    case 159:
      markup.append("  board type=BANDAI-LZ93D50\n");
      markup.append("  chip type=LZ93D50\n");
      markup.append("  chip type=24C01\n");
      break;
    }
    break;

  case  18:
    markup.append("  board type=JALECO-JF-24\n");
    markup.append("  chip type=SS88006\n");
    break;

  case  21: //VRC4a,VRC4c
  case  23: //VRC4e,VRC4f,VRC2b
  case  25: //VRC4b,VRC4d,VRC2c
    //VRC4
    markup.append("  board type=KONAMI-VRC-4\n");
    markup.append("  chip type=", submapper == 15 ? "VRC2" : "VRC4", "\n");
    if(submapper == 0) {
      switch(mapper) {
      case 21: markup.append("    pinout a0=1 a1=2\n"); break;
      case 23: markup.append("    pinout a0=0 a1=1\n"); break;
      case 25: markup.append("    pinout a0=1 a1=0\n"); break;
      }
    } else if(submapper == 15) {
      markup.append("    pinout",
        " a0=", submapper == 25,
        " a1=", submapper == 23,
      "\n");
    } else {
      markup.append("    pinout",
        " a0=", submapper & 7,
        " a1=", (submapper & 7) + (((submapper & 8) >> 2) - 1),
      "\n");
    }
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  22:
    //VRC2a
    markup.append("  board type=KONAMI-VRC-2\n");
    markup.append("  chip type=VRC2\n");
    markup.append("    pinout a0=1 a1=0\n");
    break;

  case  24:
  case  26:
    markup.append("  board type=KONAMI-VRC-6\n");
    markup.append("  chip type=VRC6\n");
    switch(mapper) {
    case 24: markup.append("    pinout a0=0 a1=1\n"); break;
    case 26: markup.append("    pinout a0=1 a1=0\n"); break;
    }
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  32:
    markup.append("  board type=IREM-G101\n");
    markup.append("  chip type=G-101\n");
    if(submapper == 1) markup.append("  mirror mode=screen-1\n");
    break;

  case  34:
    markup.append("  board type=NES-BNROM\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  65:
    markup.append("  board type=IREM-H3001\n");
    markup.append("  chip type=IF-H3001\n");
    break;

  case  66:
    markup.append("  board type=NES-GNROM\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  69:
    markup.append("  board type=SUNSOFT-5B\n");
    markup.append("  chip type=5B\n");
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  70:
    markup.append("  board type=BANDAI-74*161/161/32\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  73:
    markup.append("  board type=KONAMI-VRC-3\n");
    markup.append("  chip type=VRC3\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  75:
    markup.append("  board type=KONAMI-VRC-1\n");
    markup.append("  chip type=VRC1\n");
    break;

  case  77:
    markup.append("  board type=IREM-74*161/161/21/138\n");
    break;

  case  78:
    markup.append("  board type=",
      submapper != 3 ? "JALECO-JF-16" : "IREM-HOLYDIVER",
    "\n");
    break;

  case  85:
    markup.append("  board type=KONAMI-VRC-7\n");
    markup.append("  chip type=VRC7\n");
    if(!nes2 && !prgram) prgram = 8192;
    break;

  case  86:
    markup.append("  board type=JALECO-JF-13\n");
    markup.append("  chip type=uPD7756C\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  87:
    markup.append("  board type=JALECO-JF-09\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  97:
    markup.append("  board type=IREM-TAM-S1\n");
    markup.append("  chip type=TAM-S1\n");
    break;

  case 140:
    markup.append("  board type=JALECO-JF-14\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  76:
  case  88:
  case  95:
  case 154:
  case 206:
    switch(mapper) {
    case  76: markup.append("  board type=NAMCOT-3446\n"); break;
    case  88: markup.append("  board type=NAMCOT-3443\n"); break;
    case  95: markup.append("  board type=NAMCOT-3425\n"); break;
    case 154: markup.append("  board type=NAMCOT-3453\n"); break;
    case 206: markup.append("  board type=NAMCOT-3401\n"); break;
    // Normally 3416, but バベルの塔 (Babel no Tou) uses bankswitching despite
    // fitting entirely in the memory map, and other 32KB programs are not
    // broken by having it enabled.
    }
    markup.append("  chip type=Namcot108\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;
  }

  markup.append("  prg\n");
  if(prgrom) markup.append("    rom name=program.rom size=0x", hex(prgrom), "\n");
  if(prgram) {
    if(battery) markup.append("    ram name=save.ram size=0x", hex(prgram), "\n");
    else        markup.append("    ram size=0x", hex(prgram), "\n");
  }

  markup.append("  chr\n");
  if(chrrom) markup.append("    rom name=character.rom size=0x", hex(chrrom), "\n");
  if(chrram) markup.append("    ram size=0x", hex(chrram), "\n");

  if(pc10) {
    markup.append("  pc10\n");
    markup.append("    rom name=instructions.rom size=0x2000\n");
    markup.append("    rom name=key.rom size=0x9\n");
  }
}

auto FamicomCartridge::vsSystemHeuristic(const uint8_t* data, unsigned size) -> void {
  mapper = ((data[7] >> 4) << 4) | (data[6] >> 4);
  mirror = ((data[6] & 0x08) >> 2) | (data[6] & 0x01);
  prgrom = data[4] * 0x4000;
  chrrom = data[5] * 0x2000;
  prgram = 0u;
  chrram = chrrom == 0u ? 8192u : 0u;
  battery = data[6] & 0x02;
  vs      = data[7] & 0x01;
  nes2    = (data[7] & 0x0c) == 0x08;
  if(!nes2) {
    // ignore the last 9 bytes of headers that have "DiskDude!" or other
    // messages written there
    if(data[12] == 0 && data[13] == 0 && data[14] == 0 && data[15] == 0) {
      prgram = data[8] * 0x2000;
      region = data[9] & 0x01;
    } else {
      mapper &= 0x0f;
      vs = false;
      pc10 = false;
    }
  } else {
    pc10 = data[7] & 0x02;
    mapper |= (data[8] & 0x0f) << 8;
    submapper |= data[8] >> 4;
    prgrom += (data[9] & 0x0f) * 0x400000;
    chrrom += (data[9] >> 4) * 0x200000;
    prgram = ((data[10] & 0x0f) == 0 ? 0 : 64) << (data[10] & 0x0f); // no battery
    prgram += (data[10] >> 4 == 0 ? 0 : 64) << (data[10] >> 4); // battery
    chrram = ((data[11] & 0x0f) == 0 ? 0 : 64) << (data[11] & 0x0f); // no battery
    chrram += (data[11] >> 4 == 0 ? 0 : 64) << (data[11] >> 4); // battery
    region = data[12] & 0x01;
    ppu = data[13] & 0x0f;
  }
  if(prgram >= 0x800) prgram -= 0x800; // VS. System built-in RAM

  markup.append("cartridge\n");
  markup.append("  vs\n");
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
  markup.append("    ppu revision=", ppu_revision, "\n");
  markup.append("    controller port=2 device=joypad\n");
  markup.append("    controller port=1 device=joypad\n");

  switch(mapper) {
  default: // case  99:
    break;

  case   1: // SxROM-like
    markup.append("    chip type=MMC1B2\n");
    break;

  case   2: // UxROM-like
    markup.append("    chip type=74HC32\n");
    break;

  case 206: // NAMCOT-34xx-like
    markup.append("    chip type=108\n");
    break;
  }

  markup.append("    prg\n");
  if(prgrom) markup.append("      rom name=program.rom size=0x", hex(prgrom), "\n");
  if(prgram) {
    if(battery) markup.append("      ram name=save.ram size=0x", hex(prgram), "\n");
    else        markup.append("      ram size=0x", hex(prgram), "\n");
  }

  markup.append("    chr\n");
  if(chrrom) markup.append("      rom name=character.rom size=0x", hex(chrrom), "\n");
  if(chrram) markup.append("      ram size=0x", hex(chrram), "\n");
}