auto VDP::scanline() -> void {
  state.x = 0;
  if(++state.y >= 262) state.y = 0;

  if(state.y < screenHeight()) {
    planeA.scanline(state.y);
    window.scanline(state.y);
    planeB.scanline(state.y);
    sprite.scanline(state.y);
  }

  if(state.y == 240) scheduler.exit(Scheduler::Event::Frame);

  state.output = buffer + (state.y * 2 + 0) * 1280;
}

auto VDP::run() -> void {
  if(!io.displayEnable) return outputPixel(0);
  if(state.y >= screenHeight()) return outputPixel(0);

  auto& planeA = window.isWindowed(state.x, state.y) ? window : this->planeA;
  planeA.run(state.x, state.y);
  planeB.run(state.x, state.y);
  sprite.run(state.x, state.y);

  auto output = io.backgroundColor;
  uint2 intensity = 1;
  if(auto color = planeB.output.color) output = color;
  if(auto color = planeA.output.color) output = color;
  if(auto color = sprite.output.color) output = color;
  if(planeB.output.priority) if(auto color = planeB.output.color) output = color;
  if(planeA.output.priority) if(auto color = planeA.output.color) output = color;
  if(sprite.output.priority) if(auto color = sprite.output.color) output = color;

  outputPixel(cram.read(output) | intensity << 9);
  state.x++;
}

auto VDP::outputPixel(uint11 color) -> void {
  uint pixelWidth = screenWidth() == 256 ? 5 : 4;
  for(auto n : range(pixelWidth)) {
    state.output[   0 + n] = color;
    state.output[1280 + n] = color;
  }
  state.output += pixelWidth;
}
