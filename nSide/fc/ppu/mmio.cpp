auto PPU::read(uint16 addr, uint8 data) -> uint8 {
  switch(addr & 7) {

  case 0: case 1: case 3:
  case 5: case 6: {
    return r.mdr;
  }

  //PPUSTATUS
  case 2: {
    data = 0x00;
    data |= r.nmiFlag << 7;
    data |= r.spriteZeroHit << 6;
    switch(version) {
    default:
      data |= r.spriteOverflow << 5;
      data |= r.mdr & 0x1f;
      break;
    case Version::RC2C05_01:
    case Version::RC2C05_04: data |= 0x1b; break;
    case Version::RC2C05_02: data |= 0x3d; break;
    case Version::RC2C05_03: data |= 0x1c; break;
    }
    r.addressLatch = 0;
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
      r.mdr = readOAM(r.oamAddress);
      for(uint i = 0; i < 8; i++) r.mdrDecay[i] = 3221591;
      break;
    }
    return r.mdr;
  }

  //PPUDATA
  case 7: {
    if(enable() && (vcounter() <= 240 || vcounter() == (system.region() == System::Region::NTSC ? 261 : 311))) {
      return 0x00;
    }

    addr = r.vaddr & 0x3fff;
    if(addr <= 0x3eff) {
      r.mdr = r.busData;
    } else if(addr <= 0x3fff) {
      r.mdr = (r.mdr & 0xc0) | readCGRAM(addr);
    }
    r.busData = cartridge.readCHR(r.chrAddressBus = addr);
    r.vaddr += r.vramIncrement;
    r.chrAddressBus = r.vaddr;
    return r.mdr;
  }

  }

  return data;
}

auto PPU::write(uint16 addr, uint8 data) -> void {
  r.mdr = data;
  // Decay rate can vary depending on the system and temperature.
  // Value used here is PPU's NTSC clock rate * 0.6 rounded to nearest integer.
  for(uint i = 0; i < 8; i++) r.mdrDecay[i] = 3221591;

  switch(version) {
  case Version::RC2C05_01:
  case Version::RC2C05_02:
  case Version::RC2C05_03:
  case Version::RC2C05_04:
  case Version::RC2C05_05: if((addr & 6) == 0) addr ^= 1; break;
  }

  switch(addr & 7) {

  //PPUCTRL
  case 0: {
    r.nmiEnable     = data.bit(7);
    r.masterSelect  = data.bit(6);
    r.spriteHeight  = data.bit(5) ? 16 : 8;
    r.bgAddress     = data.bit(4) ? 0x1000 : 0x0000;
    r.objAddress    = data.bit(3) ? 0x1000 : 0x0000;
    r.vramIncrement = data.bit(2) ? 32 : 1;
    r.taddr = (r.taddr & 0x73ff) | (data.bits(1,0) << 10);
    cpu.nmiLine(r.nmiEnable && r.nmiHold && r.nmiFlag);
    return;
  }

  //PPUMASK
  case 1: {
    r.emphasis      = data.bits(7,5);
    r.objEnable     = data.bit(4);
    r.bgEnable      = data.bit(3);
    r.objEdgeEnable = data.bit(2);
    r.bgEdgeEnable  = data.bit(1);
    r.grayscale     = data.bit(0);
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
    writeOAM(r.oamAddress++, data);
    return;
  }

  //PPUSCROLL
  case 5: {
    if(r.addressLatch == 0) {
      r.xaddr = data.bits(2,0);
      r.taddr = (r.taddr & 0x7fe0) | data.bits(7,3);
    } else {
      r.taddr = (r.taddr & 0x0c1f) | (data.bits(2,0) << 12) | (data.bits(7,3) << 5);
    }
    r.addressLatch ^= 1;
    return;
  }

  //PPUADDR
  case 6: {
    if(r.addressLatch == 0) {
      r.taddr = (r.taddr & 0x00ff) | (data.bits(5,0) << 8);
    } else {
      r.taddr = (r.taddr & 0x7f00) | data;
      r.vaddr = r.taddr;
      r.chrAddressBus = r.vaddr;
    }
    r.addressLatch ^= 1;
    return;
  }

  //PPUDATA
  case 7: {
    if(enable() && (vcounter() <= 240 || vcounter() == (system.region() != System::Region::PAL ? 261 : 311))) {
      return;
    }

    addr = r.vaddr & 0x3fff;
    if(addr <= 0x3eff) {
      cartridge.writeCHR(r.chrAddressBus = addr, data);
    } else if(addr <= 0x3fff) {
      writeCGRAM(addr, data);
    }
    r.vaddr += r.vramIncrement;
    r.chrAddressBus = r.vaddr;
    return;
  }

  }
}
