auto TIA::readIO(uint6 addr, uint8 data) -> uint8 {
  switch(addr & 0x0f) {

  //CXM0P
  case 0x0: {
    return collision.m0p0 << 6 | collision.m0p1 << 7;
    break;
  }

  //CXM1P
  case 0x1: {
    return collision.m1p0 << 6 | collision.m1p1 << 7;
    break;
  }

  //CXP0FB
  case 0x2: {
    return collision.p0bl << 6 | collision.p0pf << 7;
    break;
  }

  //CXP1FB
  case 0x3: {
    return collision.p1bl << 6 | collision.p1pf << 7;
    break;
  }

  //CXM0FB
  case 0x4: {
    return collision.m0bl << 6 | collision.m0pf << 7;
    break;
  }

  //CXM1FB
  case 0x5: {
    return collision.m1bl << 6 | collision.m1pf << 7;
    break;
  }

  //CXBLPF
  case 0x6: {
    return collision.blpf << 7;
    break;
  }

  //CXPPMM
  case 0x7: {
    return collision.m0m1 << 6 | collision.p0p1 << 7;
    break;
  }

  //INPT0
  case 0x8: {
    data.bit(7) = peripherals.controllerPort1->pot0();
    break;
  }

  //INPT1
  case 0x9: {
    data.bit(7) = peripherals.controllerPort1->pot1();
    break;
  }

  //INPT2
  case 0xa: {
    data.bit(7) = peripherals.controllerPort2->pot0();
    break;
  }

  //INPT3
  case 0xb: {
    data.bit(7) = peripherals.controllerPort2->pot1();
    break;
  }

  //INPT4
  case 0xc: {
    data.bit(7) = peripherals.controllerPort1->fire();
    break;
  }

  //INPT5
  case 0xd: {
    data.bit(7) = peripherals.controllerPort2->fire();
    break;
  }

  }

  return data;
}

auto TIA::writeIO(uint6 addr, uint8 data) -> void {
  switch(addr) {

  //VSYNC
  case 0x00: {
    if(io.vsync && !data.bit(1)) io.vcounter = 0;
    io.vsync = data.bit(1);
    return;
  }

  //VBLANK
  case 0x01: {
    io.vblank = data.bit(1);
    return;
  }

  //WSYNC
  case 0x02: {
    cpu.rdyLine(0);
    return;
  }

  //RSYNC
  case 0x03: {
    io.hcounter = 0;
    return;
  }

  //NUSIZ0
  case 0x04: {
    player[0].numberSize = data.bits(0,2);
    //missile size is 2 ^ data, or {1, 2, 4, 8}[data].
    missile[0].size      = data.bits(4,5);
    return;
  }

  //NUSIZ1
  case 0x05: {
    player[1].numberSize = data.bits(0,2);
    missile[1].size      = data.bits(4,5);
    return;
  }

  //COLUP0
  case 0x06: {
    io.playerMissile0Color = data >> 1;
    return;
  }

  //COLUP1
  case 0x07: {
    io.playerMissile1Color = data >> 1;
    return;
  }

  //COLUPF
  case 0x08: {
    io.playfieldBallColor = data >> 1;
    return;
  }

  //COLUBK
  case 0x09: {
    io.backgroundColor = data >> 1;
    return;
  }

  //CTRLPF
  case 0x0a: {
    playfield.reflect        = data.bit (  0);
    playfield.score          = data.bit (  1);
    io.playfieldBallPriority = data.bit (  2);
    ball.size                = data.bits(4,5);
    return;
  }

  //REFP0
  case 0x0b: {
    player[0].reflect = data.bit(3);
    return;
  }

  //REFP1
  case 0x0c: {
    player[1].reflect = data.bit(3);
    return;
  }

  //PF0
  case 0x0d: {
    playfield.graphic0 = data.bits(4,7);
    return;
  }

  //PF1
  case 0x0e: {
    playfield.graphic1 = data.bits(0,7);
    return;
  }

  //PF2
  case 0x0f: {
    playfield.graphic2 = data.bits(0,7);
    return;
  }

  //RESP0
  case 0x10: {
    player[0].position = io.hcounter < 68 ? 0 : io.hcounter - 68;
    return;
  }

  //RESP1
  case 0x11: {
    player[1].position = io.hcounter < 68 ? 0 : io.hcounter - 68;
    return;
  }

  //RESM0
  case 0x12: {
    missile[0].position = io.hcounter < 68 ? 0 : io.hcounter - 68;
    return;
  }

  //RESM1
  case 0x13: {
    missile[1].position = io.hcounter < 68 ? 0 : io.hcounter - 68;
    return;
  }

  //RESBL
  case 0x14: {
    ball.position = io.hcounter < 68 ? 0 : io.hcounter - 68;
    return;
  }

  case 0x15: {
    return;
  }

  case 0x16: {
    return;
  }

  case 0x17: {
    return;
  }

  case 0x18: {
    return;
  }

  case 0x19: {
    return;
  }

  case 0x1a: {
    return;
  }

  //GRP0
  case 0x1b: {
    player[0].graphic = data;
    return;
  }

  //GRP1
  case 0x1c: {
    player[1].graphic = data;
    return;
  }

  //ENAM0
  case 0x1d: {
    missile[0].enable = data.bit(1);
    return;
  }

  //ENAM1
  case 0x1e: {
    missile[1].enable = data.bit(1);
    return;
  }

  //ENABL
  case 0x1f: {
    ball.enable = data.bit(1);
    return;
  }

  //HMP0
  case 0x20: {
    player[0].motion = data.bits(4,7);
    return;
  }

  //HMP1
  case 0x21: {
    player[1].motion = data.bits(4,7);
    return;
  }

  //HMM0
  case 0x22: {
    missile[0].motion = data.bits(4,7);
    return;
  }

  //HMM1
  case 0x23: {
    missile[1].motion = data.bits(4,7);
    return;
  }

  //HMBL
  case 0x24: {
    ball.motion = data.bits(4,7);
    return;
  }

  case 0x25: {
    return;
  }

  case 0x26: {
    return;
  }

  case 0x27: {
    return;
  }

  //RESMP0
  case 0x28: {
    missile[0].reset = data.bit(1);
    return;
  }

  //RESMP1
  case 0x29: {
    missile[1].reset = data.bit(1);
    return;
  }

  //HMOVE
  case 0x2a: {
    for(auto& p : player)  p.position = (p.position + p.motion + 160) % 160;
    for(auto& m : missile) m.position = (m.position + m.motion + 160) % 160;
    ball.position = (ball.position + ball.motion + 160) % 160;
    return;
  }

  //HMCLR
  case 0x2b: {
    player[0].motion = 0;
    player[1].motion = 0;
    missile[0].motion = 0;
    missile[1].motion = 0;
    ball.motion = 0;
    return;
  }

  //CXCLR
  case 0x2c: {
    collision.m0p0 = false;
    collision.m0p1 = false;
    collision.m1p0 = false;
    collision.m1p1 = false;
    collision.p0bl = false;
    collision.p0pf = false;
    collision.p1bl = false;
    collision.p1pf = false;
    collision.m0bl = false;
    collision.m0pf = false;
    collision.m1bl = false;
    collision.m1pf = false;
    collision.blpf = false;
    collision.m0m1 = false;
    collision.p0p1 = false;
    return;
  }

  }

}
