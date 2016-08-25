auto VDP::scanline() -> void {
  state.x = 0;
  if(++state.y >= 262) state.y = 0;

  if(state.y < activeHeight()) {
  }

  if(state.y == screenY() + screenHeight()) scheduler.exit(Scheduler::Event::Frame);

  state.output = buffer + (state.y - screenY()) * 256;
}

auto VDP::run() -> void {
  uint32 color;
  switch(system.model()) {
  case Model::SG1000: color = state.x / 16; break;
  case Model::MasterSystem: color = state.x / 16 | (state.y / (screenHeight() / 4)) << 4; break;
  case Model::GameGear: color = ((state.x - screenX()) * 64 / 160) | ((state.y - screenY()) * 64 / 144) << 6; break;
  }
  outputPixel(color);
  state.x++;
}

auto VDP::outputPixel(uint12 color) -> void {
  if(system.model() == Model::GameGear) {
    if((state.x < screenX() || state.x >= screenX() + screenWidth())) return;
    if((state.y < screenY() || state.y >= screenY() + screenHeight())) return;
  }
  *(state.output++) = color;
}
