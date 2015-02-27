#ifdef CONTROLLER_CPP

void BeamGun::enter() {
  unsigned prev = 0;
  while(true) {
    unsigned next = ppu.vcounter() * 341 + ppu.hcounter();

    if(lighttime > 0) {
      lighttime -= 1;
    }

    if(!offscreen) {
      unsigned target = y * 341 + x + (!system.vs() ? 0 : 128);
      if(next >= target && prev < target) {
        //CRT raster detected
        //light remains in the gun for 10-25 scanlines
        if(read_light()) lighttime = (!system.vs() ? 341 * 16 : 341 * 262);
      }
    }

    if(next < prev) {
      if(triggertime > 0) triggertime -= 1;
      //Vcounter wrapped back to zero; update cursor coordinates for start of new frame
      int nx = poll((unsigned)Input::BeamGunID::X);
      int ny = poll((unsigned)Input::BeamGunID::Y);
      nx += x;
      ny += y;
      x = max(-16, min(256 + 16, nx));
      y = max(-16, min(240 + 16, ny));
      offscreen = (x < 0 || y < 0 || x >= 256 || y >= 240);
    }

    prev = next;
    step(2);
  }
}

uint5 BeamGun::data() {
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

uint2 BeamGun::data1() {
  return 0;
}

uint5 BeamGun::data2() {
  bool newtrigger = poll((unsigned)Input::BeamGunID::Trigger);
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

bool BeamGun::read_light() {
  if(offscreen) return false;
  uint32 palette_index = ppu.output[y * 256 + x];
  unsigned color;
  switch(ppu.revision) {
  default:
    return ((palette_index & 0x20) && ((palette_index & 0x0F) < 0x0D));
  case PPU::Revision::RP2C04_0001:
    color = PPU::RP2C04_0001[palette_index & 63];
    break;
  case PPU::Revision::RP2C04_0002:
    color = PPU::RP2C04_0002[palette_index & 63];
    break;
  case PPU::Revision::RP2C04_0003:
    color = PPU::RP2C04_0003[palette_index & 63];
    break;
  case PPU::Revision::RP2C04_0004:
    color = PPU::RP2C04_0004[palette_index & 63];
    break;
  }
  if((color & 0xF00) > 0x600) return true;
  if((color & 0x0F0) > 0x060) return true;
  if((color & 0x00F) > 0x006) return true;
  return false;
}

void BeamGun::latch(bool data) {
  if(latched == data) return;
  latched = data;
  if(system.vs() && latched == 0) {
    counter = 0;
    trigger = poll((unsigned)Input::BeamGunID::Trigger);
    light = lighttime > 0;
  }
}

BeamGun::BeamGun(unsigned port):
Controller(port, (unsigned)Input::Device::BeamGun) {
  create(Controller::Enter, system.cpu_frequency());
  latched = 0;
  counter = 0;

  //center cursor onscreen
  x = 256 / 2;
  y = 240 / 2;
}

#endif
