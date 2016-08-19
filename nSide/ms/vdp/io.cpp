auto VDP::readData() -> uint8 {
  switch(io.controlCode) {
  case 0: {  //VRAM read
    return vram[io.controlAddress++];
  }

  case 1: {  //VRAM write
    return vram[io.controlAddress++];
  }

  case 2: {  //register
    return vram[io.controlAddress++];
  }

  case 3: {  //CRAM write
    return cram[(io.controlAddress++) & 0x3f];
  }

  }
  unreachable;
}

auto VDP::writeData(uint8 data) -> void {
  switch(io.controlCode) {
  case 0: {  //VRAM read
    vram[io.controlAddress++] = data;
    break;
  }

  case 1: {  //VRAM write
    vram[io.controlAddress++] = data;
    break;
  }

  case 2: {  //register
    switch((io.controlAddress++) & (system.model() == Model::SG1000 ? 7 : 15)) {
    case 0x00:
      io.externalVDP = data.bit(0);
      io.mode3       = data.bit(1);
      if(system.model() != Model::SG1000) {
        io.mode4           = data.bit(2);
        io.spriteShiftLeft = data.bit(3);
        io.lineInterrupt   = data.bit(4);
        io.hideLeft        = data.bit(5);
        io.horizontalLock  = data.bit(6);
        io.verticalLock    = data.bit(7);
      }
      break;
    case 0x01:
      io.spriteMag  = data.bit(0);
      io.spriteSize = data.bit(1);
      io.mode2      = data.bit(3);
      io.mode1      = data.bit(4);
      io.interrupt  = data.bit(5);
      io.blank      = data.bit(6);
      io.ramSize    = data.bit(7);
      break;
    case 0x02:
      io.nametableMask    = data.bit (  0);
      io.nametableAddress = data.bits(1,3) << 11;
      break;
    case 0x03:
      io.colorAddress = data << 6;
      break;
    case 0x04:
      io.patternAddress = data.bits(0,2) << 11;
      break;
    case 0x05:
      io.spriteAttributeMask    = data.bit (  0);
      io.spriteAttributeAddress = data.bits(1,6) << 8;
      break;
    case 0x06:
      io.spritePatternMask    = data.bit (  0);
      io.spritePatternAddress = data.bits(1,2) << 12;
      break;
    case 0x07:
      io.backColor = data.bits(0,3);
      io.textColor = data.bits(4,7);
      break;
    case 0x08:
      io.scrollX = data;
      break;
    case 0x09:
      io.scrollY = data;
      break;
    case 0x0a:
      io.lineCounter = data;
      break;
    case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
      break;
    }
    break;
  }

  case 3: {  //CRAM write
    cram[(io.controlAddress++) & 0x3f];
    break;
  }

  }
}

auto VDP::readControl() -> uint8 {
  io.controlLatch = false;
  uint8 data = (
    (io.fifthSprite) +  //Modes 1, 2, 3; undefined in Mode 4
    (io.spriteCollision << 5) +
    (io.spriteOverflow << 6) +
    (vblank() << 7)
  );
  io.fifthSprite     = 0;
  io.spriteCollision = false;
  io.spriteOverflow  = false;
  return data;
}

auto VDP::writeControl(uint8 data) -> void {
  if(!io.controlLatch) {
    io.controlAddress.bits( 0, 7) = data;
    io.controlLatch = true;
  } else {
    io.controlAddress.bits( 8,13) = (uint)data.bits(0,5);
    io.controlCode                =       data.bits(6,7);
    io.controlLatch = false;
  }
}
