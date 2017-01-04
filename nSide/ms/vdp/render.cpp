/*
auto VDP::scanline() -> void {
  state.x = 0;
  if(++state.y >= 262) state.y = 0;

  if(state.y < activeHeight()) {
  }

  if(state.y == screenY() + screenHeight()) scheduler.exit(Scheduler::Event::Frame);

  state.output = buffer + (state.y - screenY()) * 256;
}
*/

auto VDP::run() -> void {
  uint32 color;

  switch(system.model()) {

  case Model::SG1000: {
    color = io.hcounter >> 5;
    break;
  }

  case Model::MasterSystem: {
    color = io.hcounter >> 5 | (io.vcounter / (screenHeight() / 4)) << 4;
    break;
  }

  case Model::GameGear: {
    color = (((io.hcounter >> 1) - screenX()) * 64 / 160) | ((io.vcounter - screenY()) * 64 / 144) << 6;
    break;
  }

  }

  if(io.hcounter >> 1 < 256 && io.vcounter < 240) outputPixel(color);
}

auto VDP::outputPixel(uint12 color) -> void {
  if(system.model() == Model::GameGear) {
    if((io.hcounter >> 1 < screenX() || io.hcounter >> 1 >= screenX() + screenWidth() )) return;
    if((io.vcounter      < screenY() || io.vcounter      >= screenY() + screenHeight())) return;
  }
  buffer[io.vcounter << 8 | io.hcounter >> 1] = color;
}
