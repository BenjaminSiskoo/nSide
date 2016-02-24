struct FamicomCartridge {
  FamicomCartridge(const uint8_t* data, uint size);
  auto vsSystemHeuristic(const uint8_t* data, uint size) -> void;

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
  bool     vs;
  // NES 2.0
  bool     pc10;
  bool     nes2;
  uint submapper;
  uint ppu;
};

FamicomCartridge::FamicomCartridge(const uint8_t* data, uint size) {
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
    prgram = ((data[10] & 0x0f) == 0 ? 0 : 64) << (data[10] & 0x0f); // no battery
    prgram += (data[10] >> 4 == 0 ? 0 : 64) << (data[10] >> 4); // battery
    chrram = ((data[11] & 0x0f) == 0 ? 0 : 64) << (data[11] & 0x0f); // no battery
    chrram += (data[11] >> 4 == 0 ? 0 : 64) << (data[11] >> 4); // battery
    region = data[12] & 0x01;
    ppu = data[13] & 0x0f;
  }

  if(vs) { vsSystemHeuristic(data, size); return; }

  markup.append("board region=", region == (uint)Region::PAL ? "pal" : "ntsc", " ");

  switch(mapper) {
  default:
    if(prgram) {
      markup.append("id:HVC-FAMILYBASIC\n");
    } else if(prgrom <= 0x2000) {
      markup.append("id:NAMCOT-3301\n");
    } else if(prgrom <= 0x4000) {
      markup.append("id:NES-NROM-128\n");
    } else {
      markup.append("id:NES-NROM-256\n");
    }
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   1:
  case 155:
    if(prgram <= 0x2000) {
      if(submapper != 5 || prgrom >= 0x8000) {
        markup.append("id:NES-", prgrom <= 0x40000 ? "SNROM" : "SUROM", "\n");
      } else {
        markup.append("id:NES-SHROM\n");
      }
    } else {
      markup.append("id:", prgrom <= 0x40000 ? "NES-SOROM" : "HVC-SXROM", "\n");
    }
    markup.append("  chip type=MMC1", mapper != 155 ? "B2" : "A", "\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case   2:
  case  94:
  case 180:
         if(mapper == 94)     markup.append("id:HVC-UN1ROM\n");
    else if(prgrom <= 131072) markup.append("id:NES-UNROM\n");
    else                      markup.append("id:NES-UOROM\n");
    markup.append("  chip type=74HC", mapper != 180 ? "32" : "08", "\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   3:
  case 185:
    markup.append("id:NES-CNROM\n");
    if(mapper == 185) {
      markup.append("  security pass=0x", hex(submapper & 3), "\n");
    }
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   4:
  case 118:
  case 119:
    if(prgram != 0x400) {
      if(submapper != 3) {
        switch(mapper) {
        case   4:
          if(mirror & 2) markup.append("id:NES-TR1ROM\n");
          else           markup.append("id:NES-TLROM\n");
          break;
        case 118: markup.append("id:NES-TLSROM\n"); break;
        case 119: markup.append("id:NES-TQROM\n"); break;
        }
        markup.append("  chip type=MMC3B\n");
      } else {
        markup.append("  chip type=ACCLAIM-MC-ACC\n");
        markup.append("  chip type=MC-ACC\n");
      }
    } else {
      markup.append("id:NES-HKROM\n");
      markup.append("  chip type=MMC6n");
    }
    if(!nes2 && !prgram) prgram = 0x2000;
    if(mapper == 119 && !nes2 && !chrram) chrram = 0x2000;
    break;

  case   5:
         if(prgram ==      0) markup.append("id:NES-ELROM\n");
    else if(prgram <= 0x2000) markup.append("id:NES-EKROM\n");
    else if(prgram == 0x8000) markup.append("id:NES-EWROM\n");
    else if(prgram == 0x4000) markup.append("id:NES-ETROM\n");
    markup.append("  chip type=MMC5\n");
    markup.append("    ram name=internal.ram size=0x400 volatile\n");
    if(!nes2 && !prgram) prgram = 32768;
    break;

  case   7:
    markup.append("id:NES-AOROM\n");
    break;

  case   9:
    markup.append("id:NES-PNROM\n");
    markup.append("  chip type=MMC2\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  10:
    markup.append("id:HVC-FKROM\n");
    markup.append("  chip type=MMC4\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  13:
    markup.append("id:NES-CPROM\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  16:
  case 153:
  case 159:
    switch(mapper) {
    case  16:
      markup.append("id:BANDAI-FCG\n");
      markup.append("  chip type=LZ93D50\n");
      markup.append("  chip type=24C02\n");
      break;
    case 153:
      markup.append("id:BANDAI-JUMP2\n");
      markup.append("  chip type=LZ93D50\n");
      if(!nes2 && !prgram) prgram = 0x2000;
      break;
    case 159:
      markup.append("id:BANDAI-LZ93D50\n");
      markup.append("  chip type=LZ93D50\n");
      markup.append("  chip type=24C01\n");
      break;
    }
    break;

  case  18:
    markup.append("id:JALECO-JF-24\n");
    markup.append("  chip type=SS88006\n");
    break;

  case  19:
  case 210:
    {
      string chip_type;
      switch(mapper) {
      case  19: chip_type = "163"; break;
      case 210: chip_type = submapper == 1 ? "175" : "340"; break;
      }
      markup.append("id:NAMCOT-", chip_type, "\n");
      markup.append("  chip type=", chip_type, "\n");
    }
    if(prgram == 0x80) {
      markup.append("    ram name=sound.ram size=0x80\n");
      prgram = 0;
    } else {
      markup.append("    ram name=sound.ram size=0x80 volatile\n");
    }
    if(mapper == 210 && submapper == 1) {
      markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    }
    break;

  case  21: //VRC4a,VRC4c
  case  23: //VRC4e,VRC4f,VRC2b
  case  25: //VRC4b,VRC4d,VRC2c
    //VRC4f is not confirmed to exist.
    markup.append("id:KONAMI-VRC-4\n");
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
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  22:
    //VRC2a
    markup.append("id:KONAMI-VRC-2\n");
    markup.append("  chip type=VRC2\n");
    markup.append("    pinout a0=1 a1=0\n");
    break;

  case  24:
  case  26:
    markup.append("id:KONAMI-VRC-6\n");
    markup.append("  chip type=VRC6\n");
    switch(mapper) {
    case 24: markup.append("    pinout a0=0 a1=1\n"); break;
    case 26: markup.append("    pinout a0=1 a1=0\n"); break;
    }
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  32:
    markup.append("id:IREM-G101\n");
    markup.append("  chip type=G-101\n");
    if(submapper == 1) markup.append("  mirror mode=screen-1\n");
    break;

  case  34:
    markup.append("id:NES-BNROM\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  65:
    markup.append("id:IREM-H3001\n");
    markup.append("  chip type=IF-H3001\n");
    break;

  case  66:
    markup.append("id:NES-GNROM\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  68:
    markup.append("id:SUNSOFT-4\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  69:
    markup.append("id:SUNSOFT-5B\n");
    markup.append("  chip type=5B\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  70:
    markup.append("id:BANDAI-74*161/161/32\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  73:
    markup.append("id:KONAMI-VRC-3\n");
    markup.append("  chip type=VRC3\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  75:
    markup.append("id:KONAMI-VRC-1\n");
    markup.append("  chip type=VRC1\n");
    break;

  case  77:
    markup.append("id:IREM-74*161/161/21/138\n");
    break;

  case  78:
    markup.append("id:", submapper != 3 ? "JALECO-JF-16" : "IREM-HOLYDIVER", "\n");
    break;

  case  85:
    markup.append("id:KONAMI-VRC-7\n");
    markup.append("  chip type=VRC7\n");
    markup.append("    pinout a0=4 a1=5\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  86:
    markup.append("id:JALECO-JF-13\n");
    markup.append("  chip type=uPD7756C\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  87:
    markup.append("id:JALECO-JF-09\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  97:
    markup.append("id:IREM-TAM-S1\n");
    markup.append("  chip type=TAM-S1\n");
    break;

  case 140:
    markup.append("id:JALECO-JF-14\n");
    markup.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  76:
  case  88:
  case  95:
  case 154:
  case 206:
    switch(mapper) {
    case  76: markup.append("id:NAMCOT-3446\n"); break;
    case  88: markup.append("id:NAMCOT-3443\n"); break;
    case  95: markup.append("id:NAMCOT-3425\n"); break;
    case 154: markup.append("id:NAMCOT-3453\n"); break;
    case 206: markup.append("id:NAMCOT-3401\n"); break;
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
    else        markup.append("    ram name=work.ram size=0x", hex(prgram), " volatile\n");
  }

  markup.append("  chr\n");
  if(chrrom) markup.append("    rom name=character.rom size=0x", hex(chrrom), "\n");
  if(chrram) markup.append("    ram name=character.ram size=0x", hex(chrram), " volatile\n");

  if(pc10) {
    markup.append("  pc10\n");
    markup.append("    rom name=instructions.rom size=0x2000\n");
    markup.append("    rom name=key.rom size=0x9\n");
  }
}

auto FamicomCartridge::vsSystemHeuristic(const uint8_t* data, unsigned size) -> void {
  mapper = (data[7] & 0x80) | (data[6] >> 4);
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
      return;
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
