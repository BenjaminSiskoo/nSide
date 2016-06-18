auto PPUcounter::tick(uint clocks) -> void {
  status.hcounter += clocks;
  if(status.hcounter == 337) status.skip = 341 - lineclocks();
  if(status.hcounter == 341 - status.skip) {
    status.hcounter = 0;
    status.skip = false;
    vcounter_tick();
  }
}

//internal
auto PPUcounter::vcounter_tick() -> void {
  status.vcounter++;

  if((system.region() == System::Region::NTSC && status.vcounter == 262)
  || (system.region() != System::Region::NTSC && status.vcounter == 312)
  ) {
    status.vcounter = 0;
    status.field = !status.field;
  }
  if(scanline) scanline();
}

auto PPUcounter::field   () const -> bool   { return status.field; }
auto PPUcounter::vcounter() const -> uint16 { return status.vcounter; }
auto PPUcounter::hcounter() const -> uint16 { return status.hcounter; }

//one PPU dot = 4 CPU clocks (1/3 CPU cycle)

auto PPUcounter::lineclocks() const -> uint16 {
  if(system.region() == System::Region::NTSC && ppu.ntsc() && ppu.rasterEnable() && vcounter() == 261 && field() == 1) return 340;
  return 341;
}

auto PPUcounter::reset() -> void {
  status.field    = 0;
  status.vcounter = 0;
  status.hcounter = 0;
  status.skip     = false;
}
