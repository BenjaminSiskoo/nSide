#ifdef SYSTEM_CPP

Video video;

void Video::generate_palette(Emulator::Interface::PaletteMode mode) {
  unsigned emphasis;
  unsigned luma;
  unsigned chroma;
  for(unsigned color = 0; color < (1 << 9); color++) {
    if(mode == Emulator::Interface::PaletteMode::Literal) {
      palette[color] = color;
      continue;
    }
    switch(ppu.revision) { default:
    case PPU::Revision::RP2C02C:
    case PPU::Revision::RP2C02G:
    case PPU::Revision::RP2C07:
      if(mode == Emulator::Interface::PaletteMode::Standard) {
        palette[color] = generate_color(color, 2.0, 0.0, 1.0, 1.0, 2.2);
      } else if(mode == Emulator::Interface::PaletteMode::Channel) {
        emphasis = image::normalize((color >> 6) &  7, 3, 16);
        luma     = image::normalize((color >> 4) &  3, 2, 16);
        chroma   = image::normalize((color >> 0) & 15, 4, 16);
        palette[color] = interface->videoColor(color, 0, emphasis, luma, chroma);
      } else if(mode == Emulator::Interface::PaletteMode::Emulation) {
        palette[color] = generate_color(color, 2.0, 0.0, 1.0, 1.0, 1.8);
      }
      break;
    case PPU::Revision::RP2C03B:
    case PPU::Revision::RP2C03G:
    case PPU::Revision::RC2C03B:
    case PPU::Revision::RC2C03C:
    case PPU::Revision::RC2C05_01:
    case PPU::Revision::RC2C05_02:
    case PPU::Revision::RC2C05_03:
    case PPU::Revision::RC2C05_04:
    case PPU::Revision::RC2C05_05:
      palette[color] = retrieve_color(PPU::RP2C03[color & 63], color);
      break;
    case PPU::Revision::RP2C04_0001:
      palette[color] = retrieve_color(PPU::RP2C04_0001[color & 63], color);
      break;
    case PPU::Revision::RP2C04_0002:
      palette[color] = retrieve_color(PPU::RP2C04_0002[color & 63], color);
      break;
    case PPU::Revision::RP2C04_0003:
      palette[color] = retrieve_color(PPU::RP2C04_0003[color & 63], color);
      break;
    case PPU::Revision::RP2C04_0004:
      palette[color] = retrieve_color(PPU::RP2C04_0004[color & 63], color);
      break;
    } 
  }
  if(system.pc10()) {
    for(unsigned color = 0; color < 256; color++) {
      palette[(1 << 9) + color] = interface->videoColor((1 << 9) + color, 0,
        uclamp<16>((15 - pc10arcadeboard.cgrom[color + 0x000]) * 4369.0),
        uclamp<16>((15 - pc10arcadeboard.cgrom[color + 0x100]) * 4369.0),
        uclamp<16>((15 - pc10arcadeboard.cgrom[color + 0x200]) * 4369.0)
      );
    }
  }
}

Video::Video() {
  // * 2 for second VS. System PPU (of which 256 colors are used for PlayChoice-10)
  palette = new uint32_t[(1 << 9) * 2]();
}

Video::~Video() {
  delete[] palette;
}

//internal

uint32_t Video::generate_color(
  unsigned n, double saturation, double hue,
  double contrast, double brightness, double gamma
) {
  signed color = (n & 0x0f), level = color < 0xe ? (n >> 4) & 3 : 1;

  static const double black = 0.518, white = 1.962, attenuation = 0.746;
  static const double levels[8] = {
    0.350, 0.518, 0.962, 1.550,
    1.094, 1.506, 1.962, 1.962,
  };

  double lo_and_hi[2] = {
    levels[level + 4 * (color == 0x0)],
    levels[level + 4 * (color <  0xd)],
  };

  double y = 0.0, i = 0.0, q = 0.0;
  auto wave = [](signed p, signed color) { return (color + p + 8) % 12 < 6; };
  for(signed p = 0; p < 12; p++) {
    double spot = lo_and_hi[wave(p, color)];

    if(((n & 0x040) && wave(p, 12))
    || ((n & 0x080) && wave(p,  4))
    || ((n & 0x100) && wave(p,  8))
    ) spot *= attenuation;

    double v = (spot - black) / (white - black);

    v = (v - 0.5) * contrast + 0.5;
    v *= brightness / 12.0;

    y += v;
    i += v * std::cos((3.141592653 / 6.0) * (p + hue));
    q += v * std::sin((3.141592653 / 6.0) * (p + hue));
  }

  i *= saturation;
  q *= saturation;

  auto gammaAdjust = [=](double f) { return f < 0.0 ? 0.0 : std::pow(f, 2.2 / gamma); };
  unsigned r = 65535.0 * gammaAdjust(y +  0.946882 * i +  0.623557 * q);
  unsigned g = 65535.0 * gammaAdjust(y + -0.274788 * i + -0.635691 * q);
  unsigned b = 65535.0 * gammaAdjust(y + -1.108545 * i +  1.709007 * q);

  return interface->videoColor(n, 0, uclamp<16>(r), uclamp<16>(g), uclamp<16>(b));
}

uint32_t Video::retrieve_color(uint9_t color, unsigned index) {
  unsigned em_r = ((index >> 6) & 1) * 7;
  unsigned em_g = ((index >> 7) & 1) * 7;
  unsigned em_b = ((index >> 8) & 1) * 7;
  return interface->videoColor(index, 0,
    uclamp<16>((((color >> 6) | em_r) & 7) * 9362.25),
    uclamp<16>((((color >> 3) | em_g) & 7) * 9362.25),
    uclamp<16>((((color >> 0) | em_b) & 7) * 9362.25)
  );
}

const uint8_t Video::cursor[15 * 15] = {
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

void Video::draw_cursor(uint16_t color, int x, int y) {
  uint32_t* data = (uint32_t*)ppu.output;

  for(int cy = 0; cy < 15; cy++) {
    int vy = y + cy - 7;
    if(vy <= 0 || vy >= 240) continue;  //do not draw offscreen

    for(int cx = 0; cx < 15; cx++) {
      int vx = x + cx - 7;
      if(vx < 0 || vx >= 256) continue;  //do not draw offscreen
      uint8_t pixel = cursor[cy * 15 + cx];
      if(pixel == 0) continue;
      uint32_t pixelcolor = (pixel == 1) ? 0x0f : color;

      *((uint32_t*)data + vy * 256 + vx) = pixelcolor;
    }
  }
}

void Video::update() {
  if(configuration.controller_port2 == Input::Device::BeamGun) {
    BeamGun &device = (BeamGun&)*input.port2;
    draw_cursor(0x27, device.x, device.y);
  }
  if(configuration.expansion_port == Input::Device::BeamGun) {
    BeamGun &device = (BeamGun&)*input.expansion;
    draw_cursor(0x2d, device.x, device.y);
  }
  if(system.pc10()) return update_pc10();
  if(system.vs()) return update_vs();
  interface->videoRefresh(video.palette, ppu.output, 4 * 256, 256, 240);
}

void Video::update_pc10() {
  if(interface->information.height == 240) {
    switch(pc10arcadeboard.display) {
    case 0: // Z80
      interface->videoRefresh(video.palette + (1 << 9), pc10arcadeboard.video_output, 4 * 256, 256, 240);
      break;
    case 1: // PPU
      interface->videoRefresh(video.palette, ppu.output, 4 * 256, 256, 240);
      break;
    }
  } else if(interface->information.height == 480) {
    uint32 buffer[256 * 480];
    for(unsigned y = 0; y < 240; y++) {
      for(unsigned x = 0; x < 256; x++) {
        buffer[(y + 240) * 256 + x] = ppu.output[y * 256 + x];
        buffer[(y +   0) * 256 + x] = (1 << 9) + pc10arcadeboard.video_output[y * 256 + x];
      }
    }
    interface->videoRefresh(video.palette, buffer, 4 * 256, 256, 480);
  }
}

void Video::update_vs() {
  if(interface->information.width == 256) {
    interface->videoRefresh(video.palette, ppu.output, 4 * 256, 256, 240);
  } else if(interface->information.width == 512) {
    uint32 buffer[512 * 240];
    for(unsigned y = 0; y < 240; y++) {
      for(unsigned x = 0; x < 256; x++) {
        buffer[y * 512 + x] = ppu.output[y * 256 + x];
        buffer[y * 512 + x + 256] = 0;
      }
    }
    interface->videoRefresh(video.palette, buffer, 4 * 512, 512, 240);
  }
}

void Video::scanline() {
  unsigned y = ppu.status.ly;
  if(y >= 240) return;
}

void Video::init() {
}

#endif
