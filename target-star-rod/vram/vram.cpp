#include "../laevateinn.hpp"
VRAMViewer* vramViewer = nullptr;

VRAMViewer::VRAMViewer() {
  vramViewer = this;
  setTitle("VRAM Viewer");
  setStatusFont(Font::sans(8, "Bold"));
  setStatusVisible();

  layout.setMargin(5);
  modeLabel.setText("Mode:");
  modeSelection.append("2bpp");
  modeSelection.append("4bpp");
  modeSelection.append("8bpp");
  paletteLabel.setText("Palette:");
  autoUpdate.setText("Auto");
  update.setText("Update");
  canvas.setSize({512, 512});

  layout.append(controlLayout, {~0, 0}, 5);
    controlLayout.append(modeLabel, {0, 0}, 5);
    controlLayout.append(modeSelection, {~0, 0}, 5);
    controlLayout.append(paletteLabel, {0, 0}, 5);
    controlLayout.append(paletteSelection, {~0, 0}, 5);
    controlLayout.append(spacer, {~0, 0});
    controlLayout.append(autoUpdate, {0, 0}, 5);
    controlLayout.append(update, {80, 0});
  layout.append(canvas, {512, 512});
  append(layout);

  modeSelection.onChange = update.onActivate = { &VRAMViewer::modeChanged, this };
  paletteSelection.onChange = update.onActivate = { &VRAMViewer::paletteChanged, this };

  canvas.onMouseLeave = [&] { setStatusText(""); };
  canvas.onMouseMove = [&](Position position) {
    unsigned x = position.x, y = position.y, mode = modeSelection.selection();
    if(x >= 256 && mode >= 1) return setStatusText("");
    if(y >= 256 && mode >= 2) return setStatusText("");
    string output = { x, ", ", y, ", " };
    x /= 8, y /= 8;
    unsigned tile = 0;
    if(mode == 0) tile = y * 64 + x;
    if(mode == 1) tile = y * 32 + x;
    if(mode == 2) tile = y * 32 + x;
    output.append("Tile: 0x", hex(tile, 4L), ", Address: 0x", hex(tile * (16 << mode), 4L));
    setStatusText(output);
  };

  modeChanged();

  setGeometry({{128, 128}, layout.minimumSize()});
  windowManager->append(this, "VRAMViewer");
}

void VRAMViewer::modeChanged() {
  paletteSelection.reset();
  switch(modeSelection.selection()) {
  case 0: // 2BPP
    for(unsigned bg = 0; bg < 4; bg++) {
      for(unsigned palette = 0; palette < 8; palette++) {
        paletteSelection.append({"BG", bg, " ", palette});
      }
    }
    break;
  case 1: // 4BPP
    for(unsigned palette = 0; palette < 8; palette++) {
      paletteSelection.append({"BG ", palette});
    }
    for(unsigned palette = 0; palette < 8; palette++) {
      paletteSelection.append({"SP ", palette});
    }
    break;
  case 2: // 8BPP
    paletteSelection.append("BG");
    break;
  }
  paletteChanged();
}

void VRAMViewer::paletteChanged() {
  updateTiles();
}

void VRAMViewer::updateTiles() {
  uint32_t* dp = canvas.data();
  for(unsigned y = 0; y < 512; y++) {
    for(unsigned x = 0; x < 512; x++) {
      *dp++ = 0xff800000;
    }
  }
  dp = canvas.data();
  const uint8_t* sp = SuperFamicom::ppu.vram;

  switch(modeSelection.selection()) {
  case 0: // 2BPP
    for(unsigned tileY = 0; tileY < 64; tileY++) {
      for(unsigned tileX = 0; tileX < 64; tileX++) {
        for(unsigned y = 0; y < 8; y++) {
          uint8_t d[] = { sp[0], sp[1] };
          for(unsigned x = 0; x < 8; x++) {
            unsigned color = 0;
            color += d[0] & 0x80 ? 1 : 0;
            color += d[1] & 0x80 ? 2 : 0;
            for(auto& b : d) b <<= 1;
            //color *= 0x55;
            //color = (255u << 24) + (color << 16) + (color << 8) + (color << 0);
            color += paletteSelection.selection() << 2;
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) | SuperFamicom::video.palette[(15u << 15) | color];
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
      }
    }
    break;

  case 1: // 4BPP
    for(unsigned tileY = 0; tileY < 64; tileY++) {
      for(unsigned tileX = 0; tileX < 32; tileX++) {
        for(unsigned y = 0; y < 8; y++) {
          uint8_t d[] = { sp[0], sp[1], sp[16], sp[17] };
          for(unsigned x = 0; x < 8; x++) {
            unsigned color = 0;
            color += d[0] & 0x80 ? 1 : 0;
            color += d[1] & 0x80 ? 2 : 0;
            color += d[2] & 0x80 ? 4 : 0;
            color += d[3] & 0x80 ? 8 : 0;
            for(auto& b : d) b <<= 1;
            //color *= 0x11;
            //color = (255u << 24) + (color << 16) + (color << 8) + (color << 0);
            color += paletteSelection.selection() << 4;
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) | SuperFamicom::video.palette[(15u << 15) | color];
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        sp += 16;
      }
    }
    break;

  case 2: // 8BPP
    for(unsigned tileY = 0; tileY < 32; tileY++) {
      for(unsigned tileX = 0; tileX < 32; tileX++) {
        for(unsigned y = 0; y < 8; y++) {
          uint8_t d[] = { sp[0], sp[1], sp[16], sp[17], sp[32], sp[33], sp[48], sp[49] };
          for(unsigned x = 0; x < 8; x++) {
            unsigned color = 0;
            color += d[0] & 0x80 ?   1 : 0;
            color += d[1] & 0x80 ?   2 : 0;
            color += d[2] & 0x80 ?   4 : 0;
            color += d[3] & 0x80 ?   8 : 0;
            color += d[4] & 0x80 ?  16 : 0;
            color += d[5] & 0x80 ?  32 : 0;
            color += d[6] & 0x80 ?  64 : 0;
            color += d[7] & 0x80 ? 128 : 0;
            for(auto& b : d) b <<= 1;
            //color = (255u << 24) + (color << 16) + (color << 8) + (color << 0);
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) | SuperFamicom::video.palette[(15u << 15) | color];
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        sp += 48;
      }
    }
    break;
  }

  canvas.setData();
}
