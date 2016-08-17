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
    vram[io.controlAddress++];
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
    (io.spriteOverflow << 6) +  //Mode 4
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
