void PPUcounter::tick(unsigned clocks) {
  status.hcounter++;
  if(status.hcounter >= 340 && status.hcounter == lineclocks()) {
    status.hcounter = 0;
    vcounter_tick();
  }
}

//internal
void PPUcounter::vcounter_tick() {
  status.vcounter++;

  if((system.region() == System::Region::NTSC && status.vcounter == 262)
  || (system.region() == System::Region::PAL  && status.vcounter == 312)
  ) {
    status.vcounter = 0;
    status.field = !status.field;
  }
  if(scanline) scanline();
}

bool   PPUcounter::field   () const { return status.field; }
uint16 PPUcounter::vcounter() const { return status.vcounter; }
uint16 PPUcounter::hcounter() const { return status.hcounter; }

uint16 PPUcounter::lineclocks() const {
  if(system.region() == System::Region::NTSC && ppu.raster_enable() && vcounter() == 261 && field() == 1) return 340;
  return 341;
}

void PPUcounter::reset() {
  status.field    = 0;
  status.vcounter = 0;
  status.hcounter = 0;
}
