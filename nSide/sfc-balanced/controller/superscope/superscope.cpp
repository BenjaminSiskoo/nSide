//The Super Scope is a light-gun: it detects the CRT beam cannon position,
//and latches the counters by toggling iobit. This only works on controller
//port 2, as iobit there is connected to the PPU H/V counter latch.
//(PIO $4201.d7)

//It is obviously not possible to perfectly simulate an IR light detecting
//a CRT beam cannon, hence this class will read the PPU raster counters.

//A Super Scope can still technically be used in port 1, however it would
//require manual polling of PIO ($4201.d6) to determine when iobit was written.
//Note that no commercial game ever utilizes a Super Scope in port 1.

SuperScope::SuperScope(bool port) : Controller(port) {
  create(Controller::Enter, 21477272);
  latched = 0;
  counter = 0;

  //center cursor onscreen
  x = 256 / 2;
  y = 240 / 2;

  trigger   = false;
  cursor    = false;
  turbo     = false;
  pause     = false;
  offscreen = false;

  turbolock   = false;
  triggerlock = false;
  pauselock   = false;

  prev = 0;

  videoCursor.origin_x = 7;
  videoCursor.origin_y = 7;
  videoCursor.width = 15;
  videoCursor.height = 15;
  videoCursor.stretch_x = 2;
  videoCursor.stretch_y = 1;
  videoCursor.palette = normalPalette;
  videoCursor.data = normalData;
}

auto SuperScope::main() -> void {
  unsigned next = cpu.vcounter() * 1364 + cpu.hcounter();

  if(offscreen == false) {
    unsigned target = y * 1364 + (x + 24) * 4;
    if(next >= target && prev < target) {
      //CRT raster detected, toggle iobit to latch counters
      iobit(0);
      iobit(1);
    }
  }

  if(next < prev) {
    //Vcounter wrapped back to zero; update cursor coordinates for start of new frame
    int nx = interface->inputPoll(port, Device::SuperScope, X);
    int ny = interface->inputPoll(port, Device::SuperScope, Y);
    nx += x;
    ny += y;
    x = max(-16, min(256 + 16, nx));
    y = max(-16, min(240 + 16, ny));
    offscreen = (x < 0 || y < 0 || x >= 256 || y >= ppu.vdisp());
    videoCursor.stretch_y = ppu.interlace() + 1;
    videoCursor.x = x * videoCursor.stretch_x;
    videoCursor.y = y * videoCursor.stretch_y;
  }

  prev = next;
  step(2);
}

auto SuperScope::data() -> uint2 {
  if(counter >= 8) return 1;

  if(counter == 0) {
    //turbo is a switch; toggle is edge sensitive
    bool newturbo = interface->inputPoll(port, Device::SuperScope, Turbo);
    if(newturbo) {
      if(!turbolock) turbo = !turbo;  //toggle state
      turbolock = true;
      videoCursor.palette = !turbo ? normalPalette : turboPalette;
      videoCursor.data = !turbo ? normalData : turboData;
    } else {
      turbolock = false;
    }

    //trigger is a button
    //if turbo is active, trigger is level sensitive; otherwise, it is edge sensitive
    trigger = false;
    bool newtrigger = interface->inputPoll(port, Device::SuperScope, Trigger);
    if(newtrigger && (turbo || !triggerlock)) {
      trigger = true;
      triggerlock = true;
    } else if(!newtrigger) {
      triggerlock = false;
    }

    //cursor is a button; it is always level sensitive
    cursor = interface->inputPoll(port, Device::SuperScope, Cursor);

    //pause is a button; it is always edge sensitive
    pause = false;
    bool newpause = interface->inputPoll(port, Device::SuperScope, Pause);
    if(newpause && !pauselock) {
      pause = true;
      pauselock = true;
    } else if(!newpause) {
      pauselock = false;
    }

    offscreen = (x < 0 || y < 0 || x >= 256 || y >= ppu.vdisp());
  }

  switch(counter++) {
  case 0: return offscreen ? 0 : trigger;
  case 1: return cursor;
  case 2: return turbo;
  case 3: return pause;
  case 4: return 0;
  case 5: return 0;
  case 6: return offscreen;
  case 7: return 0;  //noise (1 = yes)
  }
}

auto SuperScope::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;
}

const uint64 SuperScope::normalPalette[3] = {
  0x0000'0000'0000'0000l, //transparent
  0xffff'0000'0000'0000l, //black
  0xffff'0000'0000'ffffl, //blue
};

const uint64 SuperScope::turboPalette[6] = {
  0x0000'0000'0000'0000l, //transparent
  0xffff'0000'0000'0000l, //black
  0xffff'ffff'0000'0000l, //red
  0xffff'ffff'3333'0000l, //red, with a little orange
  0xffff'ffff'6666'0000l, //red, with more orange
  0xffff'ffff'9999'0000l, //orange
};

const uint8 SuperScope::normalData[15 * 15] = {
  0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
  0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,
  0,0,0,1,2,2,1,2,1,2,2,1,0,0,0,
  0,0,1,2,1,1,0,1,0,1,1,2,1,0,0,
  0,1,2,1,0,0,0,1,0,0,0,1,2,1,0,
  0,1,2,1,0,0,1,2,1,0,0,1,2,1,0,
  1,2,1,0,0,1,1,2,1,1,0,0,1,2,1,
  1,2,2,1,1,2,2,2,2,2,1,1,2,2,1,
  1,2,1,0,0,1,1,2,1,1,0,0,1,2,1,
  0,1,2,1,0,0,1,2,1,0,0,1,2,1,0,
  0,1,2,1,0,0,0,1,0,0,0,1,2,1,0,
  0,0,1,2,1,1,0,1,0,1,1,2,1,0,0,
  0,0,0,1,2,2,1,2,1,2,2,1,0,0,0,
  0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,
  0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
};

const uint8 SuperScope::turboData[15 * 15] = {
  1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,
  1,5,5,5,1,0,0,0,0,0,1,5,5,5,1,
  1,5,5,4,4,1,0,0,0,1,4,4,5,5,1,
  1,5,4,4,3,1,0,0,0,1,3,4,4,5,1,
  0,1,4,3,3,2,1,0,1,2,3,3,4,1,0,
  0,0,1,1,2,2,1,0,1,2,2,1,1,0,0,
  0,0,0,0,1,1,1,0,1,1,1,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,1,1,1,0,1,1,1,0,0,0,0,
  0,0,1,1,2,2,1,0,1,2,2,1,1,0,0,
  0,1,4,3,3,2,1,0,1,2,3,3,4,1,0,
  1,5,4,4,3,1,0,0,0,1,3,4,4,5,1,
  1,5,5,4,4,1,0,0,0,1,4,4,5,5,1,
  1,5,5,5,1,0,0,0,0,0,1,5,5,5,1,
  1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,
};
