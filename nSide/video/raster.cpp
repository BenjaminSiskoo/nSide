Raster::Raster() {
  reset();
}

auto Raster::reset() -> void {
  position = video.output;
  switch(video.effects.rotation) {
  case 0: position += 0;
  case 1: position += video.height - 1; break;
  case 2: position += video.width * video.height - 1; break;
  case 3: position += (video.width - 1) * video.height; break;
  }
}

auto Raster::reset(uint x, uint y) -> void {
  position = video.output + (y * video.width + x);
}

auto Raster::pixel(uint64 color) -> void {
  auto crush = [](uint64 color) -> uint32 {
    return (
      color.byte(1) <<  0
    | color.byte(3) <<  8
    | color.byte(5) << 16
    | color.byte(7) << 24
    );
  };

  if(!video.effects.interframeBlending) {
    *position = crush(color);
  } else {
    auto a = *position;
    auto b = crush(color);
    *position = (a + b - ((a ^ b) & 0x01010101)) >> 1;
  }

  switch(effects.rotation) {
  case 0: position +=  1;           break;
  case 1: position += this->height; break;
  case 2: position -= 1;            break;
  case 3: position -= this->height; break;
  }
}
