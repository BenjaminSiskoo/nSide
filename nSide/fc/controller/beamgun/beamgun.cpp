BeamGun::BeamGun(uint port) : Controller(port, Device::BeamGun) {
  create(Controller::Enter, system.cpuFrequency());
  sprite = Emulator::video.createSprite(16, 16);
  sprite->setPixels(port == Port::Expansion ? Resource::Sprite::CrosshairGreenSmall : Resource::Sprite::CrosshairRedSmall);

  latched = 0;
  counter = 0;

  //center cursor onscreen
  x = 256 / 2;
  y = 240 / 2;

  light       = false;
  trigger     = false;
  lighttime   = 0;
  triggertime = 0;
  offscreen   = false;

  triggerlock = false;

  prev = 0;
}

BeamGun::~BeamGun() {
  Emulator::video.removeSprite(sprite);
}

auto BeamGun::main() -> void {
  uint next = ppu.vcounter() * 341 + ppu.hcounter();

  if(lighttime > 0) {
    lighttime -= 1;
  }

  if(!offscreen) {
    uint target = y * 341 + x + (!system.vs() ? 8 : 8);
    if(next >= target && prev < target) {
      //CRT raster detected
      //light remains in the gun for 10-25 scanlines
      if(readLight()) lighttime = (!system.vs() ? 341 * 16 : 341 * 262);
    }
  }

  if(next < prev) {
    if(triggertime > 0) triggertime -= 1;
    //Vcounter wrapped back to zero; update cursor coordinates for start of new frame
    int nx = poll(X);
    int ny = poll(Y);
    nx += x;
    ny += y;
    x = max(-16, min(256 + 16, nx));
    y = max(-16, min(240 + 16, ny));
    offscreen = (x < 0 || y < 0 || x >= 256 || y >= 240);
    sprite->setPosition(x - 8, y - 8);
    sprite->setVisible(true);
  }

  prev = next;
  step(3);
}

auto BeamGun::data() -> uint5 {
  if(!system.vs()) return data2();
  if(counter >= 8) return 1;
  if(latched == 1) return 0;

  switch(counter++) {
  case 0: return 0;
  case 1: return 0;
  case 2: return 0;
  case 3: return 0;
  case 4: return 1; // connected
  case 5: return 0;
  case 6: return light;
  case 7: return trigger;
  }
}

auto BeamGun::data1() -> bool {
  return 0;
}

auto BeamGun::data2() -> uint5 {
  bool newtrigger = poll(Trigger);
  if(newtrigger && !triggerlock) {
    triggertime = 2;
    triggerlock = true;
  } else if(!newtrigger) {
    triggerlock = false;
  }
  light = lighttime > 0;
  trigger = triggertime > 0;

  return (trigger << 4) | ((!light) << 3);
}

auto BeamGun::readLight() -> bool {
  if(offscreen) return false;
  uint32 paletteIndex = ppu.output[y * 256 + x];
  uint color;
  switch(ppu.revision) {
  default:
    return ((paletteIndex & 0x20) && ((paletteIndex & 0x0f) < 0x0d));
  case PPU::Revision::RP2C04_0001:
    color = PPU::RP2C04_0001[paletteIndex & 63];
    break;
  case PPU::Revision::RP2C04_0002:
    color = PPU::RP2C04_0002[paletteIndex & 63];
    break;
  case PPU::Revision::RP2C04_0003:
    color = PPU::RP2C04_0003[paletteIndex & 63];
    break;
  case PPU::Revision::RP2C04_0004:
    color = PPU::RP2C04_0004[paletteIndex & 63];
    break;
  }
  if((color & 0xf00) > 0x600) return true;
  if((color & 0x0f0) > 0x060) return true;
  if((color & 0x00f) > 0x006) return true;
  return false;
}

auto BeamGun::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  if(system.vs() && latched == 0) {
    counter = 0;
    trigger = poll(Trigger);
    light = lighttime > 0;
  }
}
