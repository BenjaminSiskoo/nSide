auto PPU::readCIRAM(uint12 addr) -> uint8 {
  if(!system.vs()) addr &= 0x7ff;
  return ciram[addr];
}

auto PPU::writeCIRAM(uint12 addr, uint8 data) -> void {
  if(!system.vs()) addr &= 0x7ff;
  ciram[addr] = data;
}

auto PPU::readCGRAM(uint5 addr) -> uint8 {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  uint8 data = cgram[addr];
  if(r.grayscale) data &= 0x30;
  return data;
}

auto PPU::writeCGRAM(uint5 addr, uint8 data) -> void {
  if((addr & 0x13) == 0x10) addr &= ~0x10;
  cgram[addr] = data;
}

auto PPU::readIO(uint16 addr, uint8 data) -> uint8 {
  switch(addr.bits(0,2)) {

  case 0: case 1: case 3:
  case 5: case 6: {
    return r.mdr;
  }

  //PPUSTATUS
  case 2: {
    data = 0x00;
    switch(version) {
    default:
      data |= r.mdr.bits(0,4);
      data |= r.spriteOverflow << 5;
      break;
    case Version::RC2C05_01:
    case Version::RC2C05_04: data |= 0x1b; break;
    case Version::RC2C05_02: data |= 0x3d; break;
    case Version::RC2C05_03: data |= 0x1c; break;
    }
    data |= r.spriteZeroHit << 6;
    data |= r.nmiFlag << 7;
    r.v.latch = 0;
    r.nmiHold = 0;
    cpu.nmiLine(r.nmiFlag = 0);
    return data;
  }

  //OAMDATA
  case 4: {
    switch(version) {
    case Version::RP2C02C:
      return r.mdr;
    default:
      r.mdr = oam[r.oamAddress];
      for(uint i = 0; i < 8; i++) r.mdrDecay[i] = 3221591;
      break;
    }
    return r.mdr;
  }

  //PPUDATA
  case 7: {
    if(enable() && (vcounter() <= 240 || vcounter() == (system.region() == System::Region::NTSC ? 261 : 311))) return 0x00;

    addr = (uint14)r.v.address;
    if(addr <= 0x3eff) {
      r.mdr = r.busData;
    } else if(addr <= 0x3fff) {
      r.mdr = (r.mdr & 0xc0) | readCGRAM(addr);
    }
    r.busData = cartridge.readCHR(r.chrAddressBus = addr);
    r.v.address += r.vramIncrement;
    r.chrAddressBus = r.v.address;
    return r.mdr;
  }

  }

  return data;
}

auto PPU::writeIO(uint16 addr, uint8 data) -> void {
  r.mdr = data;
  // Decay rate can vary depending on the system and temperature.
  // Value used here is PPU's NTSC clock rate * 0.6 rounded to nearest integer.
  for(uint i = 0; i < 8; i++) r.mdrDecay[i] = 3221591;

  switch(version) {
  case Version::RC2C05_01:
  case Version::RC2C05_02:
  case Version::RC2C05_03:
  case Version::RC2C05_04:
  case Version::RC2C05_05: if(addr.bits(1,2) == 0) addr ^= 1; break;
  }

  switch(addr.bits(0,2)) {

  //PPUCTRL
  case 0: {
    r.t.nametable   = data.bits(0,1);
    r.vramIncrement = data.bit (2) ? 32 : 1;
    r.objAddress    = data.bit (3) ? 0x1000 : 0x0000;
    r.bgAddress     = data.bit (4) ? 0x1000 : 0x0000;
    r.spriteHeight  = data.bit (5) ? 16 : 8;
    r.masterSelect  = data.bit (6);
    r.nmiEnable     = data.bit (7);
    cpu.nmiLine(r.nmiEnable && r.nmiHold && r.nmiFlag);
    return;
  }

  //PPUMASK
  case 1: {
    r.grayscale     = data.bit (0);
    r.bgEdgeEnable  = data.bit (1);
    r.objEdgeEnable = data.bit (2);
    r.bgEnable      = data.bit (3);
    r.objEnable     = data.bit (4);
    r.emphasis      = data.bits(5,7);
    return;
  }

  //PPUSTATUS
  case 2: {
    return;
  }

  //OAMADDR
  case 3: {
    if(version != Version::RP2C07) {
      // below corruption code only applies for preferred CPU-PPU alignment.
      // on an actual Famicom/NES, waiting a while after writing to OAM will
      // make this corruption happen because the OAM will have decayed at the
      // spot being written to.
      for(int i = 0; i < 8; i++) oam[((addr & 0xf800) >> 8) + i] = oam[(r.oamAddress & 0xf8) + i];
    }
    r.oamAddress = data;
    return;
  }

  //OAMDATA
  case 4: {
    if(r.oamAddress.bits(0,1) == 2) data.bits(2,4) = 0;  //clear non-existent bits (always read back as 0)
    oam[r.oamAddress++] = data;
    return;
  }

  //PPUSCROLL
  case 5: {
    if(!r.v.latch) {
      r.v.fineX = data.bits(0,2);
      r.t.tileX = data.bits(3,7);
    } else {
      r.t.fineY = data.bits(0,2);
      r.t.tileY = data.bits(3,7);
    }
    r.v.latch ^= 1;
    return;
  }

  //PPUADDR
  case 6: {
    if(!r.v.latch) {
      r.t.addressHi = data.bits(0,5);
    } else {
      r.t.addressLo = data.bits(0,7);
      r.chrAddressBus = r.v.address = r.t.address;
    }
    r.v.latch ^= 1;
    return;
  }

  //PPUDATA
  case 7: {
    if(enable() && (vcounter() <= 240 || vcounter() == (system.region() != System::Region::PAL ? 261 : 311))) return;

    addr = (uint14)r.v.address;
    if(addr <= 0x3eff) {
      cartridge.writeCHR(r.chrAddressBus = addr, data);
    } else if(addr <= 0x3fff) {
      writeCGRAM(addr, data);
    }
    r.v.address += r.vramIncrement;
    r.chrAddressBus = r.v.address;
    return;
  }

  }
}
