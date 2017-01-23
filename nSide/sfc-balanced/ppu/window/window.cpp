auto PPU::Window::power() -> void {
  io.bg1.oneEnable = false;
  io.bg1.oneInvert = false;
  io.bg1.twoEnable = false;
  io.bg1.twoInvert = false;
  io.bg1.mask = 0;
  io.bg1.aboveEnable = false;
  io.bg1.belowEnable = false;

  io.bg2.oneEnable = false;
  io.bg2.oneInvert = false;
  io.bg2.twoEnable = false;
  io.bg2.twoInvert = false;
  io.bg2.mask = 0;
  io.bg2.aboveEnable = false;
  io.bg2.belowEnable = false;

  io.bg3.oneEnable = false;
  io.bg3.oneInvert = false;
  io.bg3.twoEnable = false;
  io.bg3.twoInvert = false;
  io.bg3.mask = 0;
  io.bg3.aboveEnable = false;
  io.bg3.belowEnable = false;

  io.bg4.oneEnable = false;
  io.bg4.oneInvert = false;
  io.bg4.twoEnable = false;
  io.bg4.twoInvert = false;
  io.bg4.mask = 0;
  io.bg4.aboveEnable = false;
  io.bg4.belowEnable = false;

  io.obj.oneEnable = false;
  io.obj.oneInvert = false;
  io.obj.twoEnable = false;
  io.obj.twoInvert = false;
  io.obj.mask = 0;
  io.obj.aboveEnable = false;
  io.obj.belowEnable = false;

  io.col.oneEnable = false;
  io.col.oneInvert = false;
  io.col.twoEnable = false;
  io.col.twoInvert = false;
  io.col.mask = 0;
  io.col.aboveMask = 0;
  io.col.belowMask = 0;

  io.oneLeft = 0x00;
  io.oneRight = 0x00;
  io.twoLeft = 0x00;
  io.twoRight = 0x00;
}
