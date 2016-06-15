auto PPU::Window::reset() -> void {
  r.bg1.oneEnable = false;
  r.bg1.oneInvert = false;
  r.bg1.twoEnable = false;
  r.bg1.twoInvert = false;
  r.bg1.mask = 0;
  r.bg1.aboveEnable = false;
  r.bg1.belowEnable = false;

  r.bg2.oneEnable = false;
  r.bg2.oneInvert = false;
  r.bg2.twoEnable = false;
  r.bg2.twoInvert = false;
  r.bg2.mask = 0;
  r.bg2.aboveEnable = false;
  r.bg2.belowEnable = false;

  r.bg3.oneEnable = false;
  r.bg3.oneInvert = false;
  r.bg3.twoEnable = false;
  r.bg3.twoInvert = false;
  r.bg3.mask = 0;
  r.bg3.aboveEnable = false;
  r.bg3.belowEnable = false;

  r.bg4.oneEnable = false;
  r.bg4.oneInvert = false;
  r.bg4.twoEnable = false;
  r.bg4.twoInvert = false;
  r.bg4.mask = 0;
  r.bg4.aboveEnable = false;
  r.bg4.belowEnable = false;

  r.obj.oneEnable = false;
  r.obj.oneInvert = false;
  r.obj.twoEnable = false;
  r.obj.twoInvert = false;
  r.obj.mask = 0;
  r.obj.aboveEnable = false;
  r.obj.belowEnable = false;

  r.col.oneEnable = false;
  r.col.oneInvert = false;
  r.col.twoEnable = false;
  r.col.twoInvert = false;
  r.col.mask = 0;
  r.col.aboveMask = 0;
  r.col.belowMask = 0;

  r.oneLeft = 0x00;
  r.oneRight = 0x00;
  r.twoLeft = 0x00;
  r.twoRight = 0x00;
}
