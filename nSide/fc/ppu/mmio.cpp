auto PPU::read(uint16 addr, uint8 data) -> uint8 {
  switch(addr & 7) {

  case 0: case 1: case 3:
  case 5: case 6: {
    return status.mdr;
  }

  //PPUSTATUS
  case 2: {
    data = 0x00;
    data |= status.nmi_flag << 7;
    data |= status.sprite_zero_hit << 6;
    switch(revision) {
    default:
      data |= status.sprite_overflow << 5;
      data |= status.mdr & 0x1f;
      break;
    case Revision::RC2C05_01:
    case Revision::RC2C05_04: data |= 0x1b; break;
    case Revision::RC2C05_02: data |= 0x3d; break;
    case Revision::RC2C05_03: data |= 0x1c; break;
    }
    status.address_latch = 0;
    status.nmi_hold = 0;
    cpu.set_nmi_line(status.nmi_flag = 0);
    return data;
  }

  //OAMDATA
  case 4: {
    switch(revision) {
    case Revision::RP2C02C:
      return status.mdr;
    default:
      status.mdr = oamRead(status.oam_addr);
      for(uint i = 0; i < 8; i++) status.mdr_decay[i] = 3221591;
      break;
    }
    return status.mdr;
  }

  //PPUDATA
  case 7: {
    if(raster_enable() && (vcounter() <= 240 || vcounter() == (system.region() == System::Region::NTSC ? 261 : 311))) {
      return 0x00;
    }

    addr = status.vaddr & 0x3fff;
    if(addr <= 0x3eff) {
      status.mdr = status.bus_data;
    } else if(addr <= 0x3fff) {
      status.mdr = (status.mdr & 0xc0) | cgramRead(addr);
    }
    status.bus_data = cartridge.chr_read(status.chr_abus = addr);
    status.vaddr += status.vram_increment;
    status.chr_abus = status.vaddr;
    return status.mdr;
  }

  }

  return data;
}

auto PPU::write(uint16 addr, uint8 data) -> void {
  status.mdr = data;
  // Decay rate can vary depending on the system and temperature.
  // Value used here is PPU's NTSC clock rate * 0.6 rounded to nearest integer.
  for(uint i = 0; i < 8; i++) status.mdr_decay[i] = 3221591;

  switch(revision) {
  case Revision::RC2C05_01:
  case Revision::RC2C05_02:
  case Revision::RC2C05_03:
  case Revision::RC2C05_04:
  case Revision::RC2C05_05: if((addr & 6) == 0) addr ^= 1; break;
  }

  switch(addr & 7) {

  //PPUCTRL
  case 0: {
    status.nmi_enable     = data.bit(7);
    status.master_select  = data.bit(6);
    status.sprite_size    = data.bit(5);
    status.bg_addr        = data.bit(4) ? 0x1000 : 0x0000;
    status.sprite_addr    = data.bit(3) ? 0x1000 : 0x0000;
    status.vram_increment = data.bit(2) ? 32 : 1;
    status.taddr = (status.taddr & 0x73ff) | (data.bits(1,0) << 10);
    cpu.set_nmi_line(status.nmi_enable && status.nmi_hold && status.nmi_flag);
    return;
  }

  //PPUMASK
  case 1: {
    status.emphasis = data.bits(7,5);
    if(revision == Revision::RP2C07) {
      status.emphasis =
        ((status.emphasis & 1) << 1) | // swap red
        ((status.emphasis & 2) >> 1) | // and green
        ((status.emphasis & 4)     );
    }
    status.sprite_enable      = data.bit(4);
    status.bg_enable          = data.bit(3);
    status.sprite_edge_enable = data.bit(2);
    status.bg_edge_enable     = data.bit(1);
    status.grayscale          = data.bit(0);
    return;
  }

  //PPUSTATUS
  case 2: {
    return;
  }

  //OAMADDR
  case 3: {
    if(revision != Revision::RP2C07) {
      // below corruption code only applies for preferred CPU-PPU alignment.
      // on an actual Famicom/NES, waiting a while after writing to OAM will
      // make this corruption happen because the OAM will have decayed at the
      // spot being written to.
      for(int i = 0; i < 8; i++) oam[((addr & 0xf800) >> 8) + i] = oam[(status.oam_addr & 0xf8) + i];
    }
    status.oam_addr = data;
    return;
  }

  //OAMDATA
  case 4: {
    oamWrite(status.oam_addr++, data);
    return;
  }

  //PPUSCROLL
  case 5: {
    if(status.address_latch == 0) {
      status.xaddr = data.bits(2,0);
      status.taddr = (status.taddr & 0x7fe0) | data.bits(7,3);
    } else {
      status.taddr = (status.taddr & 0x0c1f) | (data.bits(2,0) << 12) | (data.bits(7,3) << 5);
    }
    status.address_latch ^= 1;
    return;
  }

  //PPUADDR
  case 6: {
    if(status.address_latch == 0) {
      status.taddr = (status.taddr & 0x00ff) | (data.bits(5,0) << 8);
    } else {
      status.taddr = (status.taddr & 0x7f00) | data;
      status.vaddr = status.taddr;
      status.chr_abus = status.vaddr;
    }
    status.address_latch ^= 1;
    return;
  }

  //PPUDATA
  case 7: {
    if(raster_enable() && (vcounter() <= 240 || vcounter() == (system.region() != System::Region::PAL ? 261 : 311))) {
      return;
    }

    addr = status.vaddr & 0x3fff;
    if(addr <= 0x3eff) {
      cartridge.chr_write(status.chr_abus = addr, data);
    } else if(addr <= 0x3fff) {
      cgramWrite(addr, data);
    }
    status.vaddr += status.vram_increment;
    status.chr_abus = status.vaddr;
    return;
  }

  }
}
