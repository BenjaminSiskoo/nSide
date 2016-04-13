Justifier::Justifier(bool port, bool chained):
Controller(port),
chained(chained),
device(chained == false ? Device::Justifier : Device::Justifiers)
{
  create(Controller::Enter, 21477272);
  latched = 0;
  counter = 0;
  active = 0;
  prev = 0;

  player1.x = 256 / 2;
  player1.y = 240 / 2;
  player1.trigger = false;
  player2.start = false;

  player2.x = 256 / 2;
  player2.y = 240 / 2;
  player2.trigger = false;
  player2.start = false;

  if(chained == false) {
    player2.x = -1;
    player2.y = -1;
  } else {
    player1.x -= 16;
    player2.x += 16;
  }

  cursor1.origin_x = 7;
  cursor1.origin_y = 7;
  cursor1.width = 15;
  cursor1.height = 15;
  cursor1.stretch_x = 2;
  cursor1.stretch_y = 1;
  cursor1.palette = cursorPalette1;
  cursor1.data = cursorData;
  cursor2.origin_x = 7;
  cursor2.origin_y = 7;
  cursor2.width = 15;
  cursor2.height = 15;
  cursor2.stretch_x = 2;
  cursor2.stretch_y = 1;
  cursor2.palette = cursorPalette2;
  cursor2.data = cursorData;
}

auto Justifier::main() -> void {
  unsigned next = cpu.vcounter() * 1364 + cpu.hcounter();

  signed x = (active == 0 ? player1.x : player2.x), y = (active == 0 ? player1.y : player2.y);
  bool offscreen = (x < 0 || y < 0 || x >= 256 || y >= ppu.vdisp());

  if(offscreen == false) {
    unsigned target = y * 1364 + (x + 24) * 4;
    if(next >= target && prev < target) {
      //CRT raster detected, toggle iobit to latch counters
      iobit(0);
      iobit(1);
    }
  }

  if(next < prev) {
    int nx1 = interface->inputPoll(port, device, 0 + X);
    int ny1 = interface->inputPoll(port, device, 0 + Y);
    nx1 += player1.x;
    ny1 += player1.y;
    player1.x = max(-16, min(256 + 16, nx1));
    player1.y = max(-16, min(240 + 16, ny1));
    cursor1.stretch_y = ppu.interlace() + 1;
    cursor1.x = player1.x * cursor1.stretch_x;
    cursor1.y = player1.y * cursor1.stretch_y;
  }

  if(next < prev && chained) {
    int nx2 = interface->inputPoll(port, device, 4 + X);
    int ny2 = interface->inputPoll(port, device, 4 + Y);
    nx2 += player2.x;
    ny2 += player2.y;
    player2.x = max(-16, min(256 + 16, nx2));
    player2.y = max(-16, min(240 + 16, ny2));
    cursor2.stretch_y = ppu.interlace() + 1;
    cursor2.x = player2.x * cursor2.stretch_x;
    cursor2.y = player2.y * cursor2.stretch_y;
  }

  prev = next;
  step(2);
}

auto Justifier::data() -> uint2 {
  if(counter >= 32) return 1;

  if(counter == 0) {
    player1.trigger = interface->inputPoll(port, device, 0 + Trigger);
    player1.start   = interface->inputPoll(port, device, 0 + Start);
  }

  if(counter == 0 && chained) {
    player2.trigger = interface->inputPoll(port, device, 4 + Trigger);
    player2.start   = interface->inputPoll(port, device, 4 + Start);
  }

  switch(counter++) {
  case  0: return 0;
  case  1: return 0;
  case  2: return 0;
  case  3: return 0;
  case  4: return 0;
  case  5: return 0;
  case  6: return 0;
  case  7: return 0;
  case  8: return 0;
  case  9: return 0;
  case 10: return 0;
  case 11: return 0;

  case 12: return 1;  //signature
  case 13: return 1;  // ||
  case 14: return 1;  // ||
  case 15: return 0;  // ||

  case 16: return 0;
  case 17: return 1;
  case 18: return 0;
  case 19: return 1;
  case 20: return 0;
  case 21: return 1;
  case 22: return 0;
  case 23: return 1;

  case 24: return player1.trigger;
  case 25: return player2.trigger;
  case 26: return player1.start;
  case 27: return player2.start;
  case 28: return active;

  case 29: return 0;
  case 30: return 0;
  case 31: return 0;
  }
}

auto Justifier::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;
  if(latched == 0) active = !active;  //toggle between both controllers, even when unchained
}

const uint64 Justifier::cursorPalette1[3] = {0x0000000000000000l, 0xffff000000000000l, 0xffffffff00000000l};
const uint64 Justifier::cursorPalette2[6] = {0x0000000000000000l, 0xffff000000000000l, 0xffff0000bfff0000l};
const uint8 Justifier::cursorData[15 * 15] = {
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
