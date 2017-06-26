struct FamicomCartridge {
  FamicomCartridge(string location, uint8* data, uint size);

  string manifest;

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

FamicomCartridge::FamicomCartridge(string location, uint8* data, uint size) {
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
  vs      = data[7].bit(0);
  nes2    = (data[7] & 0x0c) == 0x08;
  if(!nes2) {
    //ignore the last 9 bytes of headers that have "DiskDude!" or other
    //messages written there
    if(data[12] == 0 && data[13] == 0 && data[14] == 0 && data[15] == 0) {
      prgram = data[8] * 0x2000;
      region = data[9].bit(0);
    } else {
      mapper &= 0x0f;
      vs = false;
    }
    pc10 = false;
  } else {
    pc10 = data[7].bit(1);
    mapper |= data[8].bits(0,3) << 8;
    submapper = data[8].bits(4,7);
    prgrom += data[9].bits(0,3) * 0x400000;
    chrrom += data[9].bits(4,7) * 0x200000;
    prgram  = (data[10].bits(0,3) == 0 ? 0 : 64) << data[10].bits(0,3);  //no battery
    prgram += (data[10].bits(4,7) == 0 ? 0 : 64) << data[10].bits(4,7);  //battery
    chrram  = (data[11].bits(0,3) == 0 ? 0 : 64) << data[11].bits(0,3);  //no battery
    chrram += (data[11].bits(4,7) == 0 ? 0 : 64) << data[11].bits(4,7);  //battery
    region = data[12].bit(0);
    ppu = data[13].bits(0,3);
  }

  manifest.append("board region=", region == (uint)Region::PAL ? "pal" : "ntsc-j", " ");

  switch(mapper) {
  default:
    if(prgram) {
      manifest.append("id:HVC-FAMILYBASIC\n");
    } else if(prgrom <= 0x2000) {
      manifest.append("id:NAMCOT-3301\n");
    } else if(prgrom <= 0x4000) {
      manifest.append("id:NES-NROM-128\n");
    } else {
      manifest.append("id:NES-NROM-256\n");
    }
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   1:
  case 155:
    if(prgram <= 0x2000) {
      if(submapper != 5 || prgrom >= 0x8000) {
        manifest.append("id:NES-", prgrom <= 0x40000 ? "SNROM" : "SUROM", "\n");
      } else {
        manifest.append("id:NES-SHROM\n");
      }
    } else {
      manifest.append("id:", prgrom <= 0x40000 ? "NES-SOROM" : "HVC-SXROM", "\n");
    }
    manifest.append("  chip type=MMC1", mapper != 155 ? "B2" : "A", "\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case   2:
  case  94:
  case 180:
         if(mapper == 94)     manifest.append("id:HVC-UN1ROM\n");
    else if(prgrom <= 131072) manifest.append("id:NES-UNROM\n");
    else                      manifest.append("id:NES-UOROM\n");
    manifest.append("  chip type=74HC", mapper != 180 ? "32" : "08", "\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   3:
  case 185:
    manifest.append("id:NES-CNROM\n");
    if(mapper == 185) {
      manifest.append("  security pass=0x", hex(submapper & 3), "\n");
    }
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case   4:
  case 118:
  case 119:
    if(prgram != 0x400) {
      if(submapper != 3) {
        switch(mapper) {
        case   4:
          if(mirror & 2) manifest.append("id:NES-TR1ROM\n");
          else           manifest.append("id:NES-TLROM\n");
          break;
        case 118: manifest.append("id:NES-TLSROM\n"); break;
        case 119: manifest.append("id:NES-TQROM\n"); break;
        }
        manifest.append("  chip type=MMC3B\n");
      } else {
        manifest.append("  chip type=ACCLAIM-MC-ACC\n");
        manifest.append("  chip type=MC-ACC\n");
      }
    } else {
      manifest.append("id:NES-HKROM\n");
      manifest.append("  chip type=MMC6n");
    }
    if(!nes2 && !prgram) prgram = 0x2000;
    if(mapper == 119 && !nes2 && !chrram) chrram = 0x2000;
    break;

  case   5:
         if(prgram ==      0) manifest.append("id:NES-ELROM\n");
    else if(prgram <= 0x2000) manifest.append("id:NES-EKROM\n");
    else if(prgram == 0x8000) manifest.append("id:NES-EWROM\n");
    else if(prgram == 0x4000) manifest.append("id:NES-ETROM\n");
    manifest.append("  chip type=MMC5\n");
    manifest.append("    ram name=internal.ram size=0x400 volatile\n");
    if(!nes2 && !prgram) prgram = 32768;
    break;

  case   7:
    manifest.append("id:NES-AOROM\n");
    break;

  case   9:
    manifest.append("id:NES-PNROM\n");
    manifest.append("  chip type=MMC2\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  10:
    manifest.append("id:HVC-FKROM\n");
    manifest.append("  chip type=MMC4\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  13:
    manifest.append("id:NES-CPROM\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  16:
  case 153:
  case 159:
    switch(mapper) {
    case  16:
      manifest.append("id:BANDAI-FCG\n");
      manifest.append("  chip type=LZ93D50\n");
      manifest.append("  chip type=24C02\n");
      break;
    case 153:
      manifest.append("id:BANDAI-JUMP2\n");
      manifest.append("  chip type=LZ93D50\n");
      if(!nes2 && !prgram) prgram = 0x2000;
      break;
    case 159:
      manifest.append("id:BANDAI-LZ93D50\n");
      manifest.append("  chip type=LZ93D50\n");
      manifest.append("  chip type=24C01\n");
      break;
    }
    break;

  case  18:
    manifest.append("id:JALECO-JF-24\n");
    manifest.append("  chip type=SS88006\n");
    break;

  case  19:
  case 210:
    {
      string chip_type;
      switch(mapper) {
      case  19: chip_type = "163"; break;
      case 210: chip_type = submapper == 1 ? "175" : "340"; break;
      }
      manifest.append("id:NAMCOT-", chip_type, "\n");
      manifest.append("  chip type=", chip_type, "\n");
    }
    if(prgram == 0x80) {
      manifest.append("    ram name=sound.ram size=0x80\n");
      prgram = 0;
    } else {
      manifest.append("    ram name=sound.ram size=0x80 volatile\n");
    }
    if(mapper == 210 && submapper == 1) {
      manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    }
    break;

  case  21: //VRC4a,VRC4c
  case  23: //VRC4e,VRC4f,VRC2b
  case  25: //VRC4b,VRC4d,VRC2c
    //VRC4f is not confirmed to exist.
    manifest.append("id:KONAMI-VRC-4\n");
    manifest.append("  chip type=", submapper == 15 ? "VRC2" : "VRC4", "\n");
    if(submapper == 0) {
      switch(mapper) {
      case 21: manifest.append("    pinout a0=1 a1=2\n"); break;
      case 23: manifest.append("    pinout a0=0 a1=1\n"); break;
      case 25: manifest.append("    pinout a0=1 a1=0\n"); break;
      }
    } else if(submapper == 15) {
      manifest.append("    pinout",
        " a0=", submapper == 25,
        " a1=", submapper == 23,
      "\n");
    } else {
      manifest.append("    pinout",
        " a0=", submapper & 7,
        " a1=", (submapper & 7) + (((submapper & 8) >> 2) - 1),
      "\n");
    }
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  22:
    //VRC2a
    manifest.append("id:KONAMI-VRC-2\n");
    manifest.append("  chip type=VRC2\n");
    manifest.append("    pinout a0=1 a1=0\n");
    break;

  case  24:
  case  26:
    manifest.append("id:KONAMI-VRC-6\n");
    manifest.append("  chip type=VRC6\n");
    switch(mapper) {
    case 24: manifest.append("    pinout a0=0 a1=1\n"); break;
    case 26: manifest.append("    pinout a0=1 a1=0\n"); break;
    }
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  32:
    manifest.append("id:IREM-G101\n");
    manifest.append("  chip type=G-101\n");
    if(submapper == 1) manifest.append("  mirror mode=screen-1\n");
    break;

  case  34:
    manifest.append("id:NES-BNROM\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  65:
    manifest.append("id:IREM-H3001\n");
    manifest.append("  chip type=IF-H3001\n");
    break;

  case  66:
    manifest.append("id:NES-GNROM\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  68:
    manifest.append("id:SUNSOFT-4\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  69:
    manifest.append("id:SUNSOFT-5B\n");
    manifest.append("  chip type=5B\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  70:
    manifest.append("id:BANDAI-74*161/161/32\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  73:
    manifest.append("id:KONAMI-VRC-3\n");
    manifest.append("  chip type=VRC3\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  75:
    manifest.append("id:KONAMI-VRC-1\n");
    manifest.append("  chip type=VRC1\n");
    break;

  case  77:
    manifest.append("id:IREM-74*161/161/21/138\n");
    break;

  case  78:
    manifest.append("id:", submapper != 3 ? "JALECO-JF-16" : "IREM-HOLYDIVER", "\n");
    break;

  case  85:
    manifest.append("id:KONAMI-VRC-7\n");
    manifest.append("  chip type=VRC7\n");
    manifest.append("    pinout a0=4 a1=5\n");
    if(!nes2 && !prgram) prgram = 0x2000;
    break;

  case  86:
    manifest.append("id:JALECO-JF-13\n");
    manifest.append("  chip type=uPD7756C\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  87:
    manifest.append("id:JALECO-JF-09\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  97:
    manifest.append("id:IREM-TAM-S1\n");
    manifest.append("  chip type=TAM-S1\n");
    break;

  case 140:
    manifest.append("id:JALECO-JF-14\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;

  case  76:
  case  88:
  case  95:
  case 154:
  case 206:
    switch(mapper) {
    case  76: manifest.append("id:NAMCOT-3446\n"); break;
    case  88: manifest.append("id:NAMCOT-3443\n"); break;
    case  95: manifest.append("id:NAMCOT-3425\n"); break;
    case 154: manifest.append("id:NAMCOT-3453\n"); break;
    case 206: manifest.append("id:NAMCOT-3401\n"); break;
    // Normally 3416, but バベルの塔 (Babel no Tou) uses bankswitching despite
    // fitting entirely in the memory map, and other 32KB programs are not
    // broken by having it enabled.
    }
    manifest.append("  chip type=Namcot108\n");
    manifest.append("  mirror mode=", mirror == 0 ? "horizontal" : "vertical", "\n");
    break;
  }

  manifest.append("  prg\n");
  if(prgrom) manifest.append("    rom name=program.rom size=0x", hex(prgrom), "\n");
  if(prgram) {
    if(battery) manifest.append("    ram name=save.ram size=0x", hex(prgram), "\n");
    else        manifest.append("    ram name=work.ram size=0x", hex(prgram), " volatile\n");
  }

  manifest.append("  chr\n");
  if(chrrom) manifest.append("    rom name=character.rom size=0x", hex(chrrom), "\n");
  if(chrram) manifest.append("    ram name=character.ram size=0x", hex(chrram), " volatile\n");

  if(pc10) {
    manifest.append("  pc10\n");
    manifest.append("    rom name=instructions.rom size=0x2000\n");
    manifest.append("    rom name=key.rom size=0x9\n");
  }

  manifest.append("\n");
  manifest.append("information\n");
  manifest.append("  region: ", region == 0 ? "NTSC" : "PAL", "\n");
  manifest.append("  title:  ", Location::prefix(location), "\n");
  manifest.append("  sha256: ", Hash::SHA256(data, size).digest(), "\n");
  manifest.append("\n");
  manifest.append("note: heuristically generated by cart-pal\n");
}
