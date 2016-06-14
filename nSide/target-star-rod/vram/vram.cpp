#include "../laevateinn.hpp"
unique_pointer<VRAMViewer> vramViewer;

VRAMViewer::VRAMViewer() {
  vramViewer = this;
  setTitle("VRAM Viewer");
  statusBar.setFont(Font().setBold());
  statusBar.setVisible();

  layout.setMargin(5);
  modeLabel.setText("Mode:");
  modeSelection.append(ComboButtonItem().setText("2bpp"));
  modeSelection.append(ComboButtonItem().setText("4bpp"));
  modeSelection.append(ComboButtonItem().setText("8bpp"));
  modeSelection.append(ComboButtonItem().setText("Mode 7"));
  paletteLabel.setText("Palette:");
  autoUpdate.setText("Auto");
  update.setText("Update");
  canvas.setSize({512, 512});

  modeSelection.onChange({ &VRAMViewer::modeChanged, this });
  paletteSelection.onChange({ &VRAMViewer::paletteChanged, this });
  update.onActivate({ &VRAMViewer::updateTiles, this });

  canvas.onMouseLeave([&] { statusBar.setText(""); });
  canvas.onMouseMove([&](Position position) {
    uint x = position.x(), y = position.y(), mode = modeSelection.selected().offset();
    if((x >= 256 && mode >= 1)) { statusBar.setText(""); return; }
    if((y >= 256 && mode >= 2)) { statusBar.setText(""); return; }
    if((x >= 128 && mode >= 3)) { statusBar.setText(""); return; }
    string output = { x, ", ", y, ", " };
    x /= 8, y /= 8;
    uint tile = 0;
    if(mode == 0) tile = y * 64 + x;
    if(mode == 1) tile = y * 32 + x;
    if(mode == 2) tile = y * 32 + x;
    if(mode == 3) tile = y * 16 + x;
    output.append("Tile: 0x", hex(tile, 4L), ", Address: 0x", hex(tile * (16 << mode), 4L));
    statusBar.setText(output);
  });

  modeChanged();

  setGeometry({{128, 128}, layout.minimumSize()});
  windowManager->append(this, "VRAMViewer");
}

auto VRAMViewer::modeChanged() -> void {
  paletteSelection.reset();
  switch(modeSelection.selected().offset()) {
  case 0: // 2BPP
    for(uint bg = 0; bg < 4; bg++) {
      for(uint palette = 0; palette < 8; palette++) {
        paletteSelection.append(ComboButtonItem().setText({"BG", bg, " ", palette}));
      }
    }
    break;
  case 1: // 4BPP
    for(uint palette = 0; palette < 8; palette++) {
      paletteSelection.append(ComboButtonItem().setText({"BG ", palette}));
    }
    for(uint palette = 0; palette < 8; palette++) {
      paletteSelection.append(ComboButtonItem().setText({"SP ", palette}));
    }
    break;
  case 2: // 8BPP
  case 3: // Mode 7
    paletteSelection.append(ComboButtonItem().setText("BG"));
    break;
  }
  paletteChanged();
}

auto VRAMViewer::paletteChanged() -> void {
  updateTiles();
}

auto VRAMViewer::updateTiles() -> void {
  uint32_t* dp = canvas.data();
  for(uint y = 0; y < 512; y++) {
    for(uint x = 0; x < 512; x++) {
      *dp++ = 0xff800000;
    }
  }
  dp = canvas.data();
  const uint8* sp = SuperFamicom::ppu.memory.vram;

  switch(modeSelection.selected().offset()) {
  case 0: // 2BPP
    for(uint tileY = 0; tileY < 64; tileY++) {
      for(uint tileX = 0; tileX < 64; tileX++) {
        for(uint y = 0; y < 8; y++) {
          uint8 d[] = { sp[0], sp[1] };
          for(uint x = 0; x < 8; x++) {
            uint color = 0;
            color += d[0] & 0x80 ? 1 : 0;
            color += d[1] & 0x80 ? 2 : 0;
            for(auto& b : d) b <<= 1;
            //color *= 0x55;
            //color = (255u << 24) + (color << 16) + (color << 8) + (color << 0);
            color += paletteSelection.selected().offset() << 2;
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
      }
    }
    break;

  case 1: // 4BPP
    for(uint tileY = 0; tileY < 64; tileY++) {
      for(uint tileX = 0; tileX < 32; tileX++) {
        for(uint y = 0; y < 8; y++) {
          uint8 d[] = { sp[0], sp[1], sp[16], sp[17] };
          for(uint x = 0; x < 8; x++) {
            uint color = 0;
            color += d[0] & 0x80 ? 1 : 0;
            color += d[1] & 0x80 ? 2 : 0;
            color += d[2] & 0x80 ? 4 : 0;
            color += d[3] & 0x80 ? 8 : 0;
            for(auto& b : d) b <<= 1;
            //color *= 0x11;
            //color = (255u << 24) + (color << 16) + (color << 8) + (color << 0);
            color += paletteSelection.selected().offset() << 4;
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        sp += 16;
      }
    }
    break;

  case 2: // 8BPP
    for(uint tileY = 0; tileY < 32; tileY++) {
      for(uint tileX = 0; tileX < 32; tileX++) {
        for(uint y = 0; y < 8; y++) {
          uint8 d[] = { sp[0], sp[1], sp[16], sp[17], sp[32], sp[33], sp[48], sp[49] };
          for(uint x = 0; x < 8; x++) {
            uint color = 0;
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
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        sp += 48;
      }
    }
    break;

  case 3: // Mode 7
    for(uint tileY = 0; tileY < 32; tileY++) {
      for(uint tileX = 0; tileX < 16; tileX++) {
        for(uint y = 0; y < 8; y++) {
          for(uint x = 0; x < 8; x++) {
            uint color = 0;
            color += sp[x << 1 | 1];
            //color = (255u << 24) + (color << 16) + (color << 8) + (color << 0);
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 16;
        }
      }
    }
    break;
  }

  canvas.update();
}
