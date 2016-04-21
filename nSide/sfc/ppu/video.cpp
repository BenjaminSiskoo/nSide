//note: this source file is currently unused
//saved temporarily only for reference

auto Video::refresh() -> void {
  auto output = this->output() + 16 * 512;  //add offset for overscan
  auto& palette = settings.colorEmulation ? paletteEmulation : paletteStandard;

  if(settings.scanlineEmulation) {
    for(uint y = 0; y < 240; y++) {
      auto sourceLo = ppu.output + y * 1024;
      auto sourceHi = ppu.output + y * 1024 + 512;
      auto targetLo = output + y * 1024;
      auto targetHi = output + y * 1024 + 512;
      if(!ppu.interlace()) {
        for(uint x = 0; x < 512; x++) {
          auto color = palette[*sourceLo++];
          *targetLo++ = color;
          *targetHi++ = (255 << 24) | ((color & 0xfefefe) >> 1);
        }
      } else if(!ppu.field()) {
        for(uint x = 0; x < 512; x++) {
          auto color = palette[*sourceHi++];
          *targetLo++ = palette[*sourceLo++];
          *targetHi++ = (255 << 24) | ((color & 0xfefefe) >> 1);
        }
      } else {
        for(uint x = 0; x < 512; x++) {
          auto color = palette[*sourceLo++];
          *targetLo++ = (255 << 24) | ((color & 0xfefefe) >> 1);
          *targetHi++ = palette[*sourceHi++];
        }
      }
    }
  } else {
    for(uint y = 0; y < 240; y++) {
      auto sourceLo = ppu.output + y * 1024;
      auto sourceHi = ppu.output + y * 1024 + 512;
      auto targetLo = output + y * 1024;
      auto targetHi = output + y * 1024 + 512;
      if(!ppu.interlace()) {
        for(uint x = 0; x < 512; x++) {
          auto color = palette[*sourceLo++];
          *targetLo++ = color;
          *targetHi++ = color;
        }
      } else {
        for(uint x = 0; x < 512; x++) {
          *targetLo++ = palette[*sourceLo++];
          *targetHi++ = palette[*sourceHi++];
        }
      }
    }
  }

  if(settings.blurEmulation) {
    for(uint y = 0; y < 480; y++) {
      auto target = output + y * 512;
      for(uint x = 0; x < 512; x++) {
        auto a = target[x];
        auto b = target[x + (x != 511)];
        target[x] = (a + b - ((a ^ b) & 0x01010101)) >> 1;
      }
    }
  }

  drawCursors();
  interface->videoRefresh(output - (ppu.overscan() ? 0 : 7 * 1024), 512 * sizeof(uint32), 512, 480);
}
