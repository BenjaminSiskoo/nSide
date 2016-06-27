#include "../laevateinn.hpp"
unique_pointer<VRAMViewer> vramViewer;

VRAMViewer::VRAMViewer() {
  vramViewer = this;
  setTitle("VRAM Viewer");
  statusBar.setFont(Font().setBold());
  statusBar.setVisible();

  layout.setMargin(5);
  modeLabel.setText("Mode:");
  modeSelection.append(ComboButtonItem().setText("2BPP"));
  modeSelection.append(ComboButtonItem().setText("4BPP"));
  modeSelection.append(ComboButtonItem().setText("8BPP"));
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
    uint tile = 0, address = 0;
    switch(mode) {
    case SuperFamicom::PPU::Background::Mode::BPP2:
      tile = y * 64 + x;
      address = tile * 16;
      break;
    case SuperFamicom::PPU::Background::Mode::BPP4:
      tile = y * 32 + x;
      address = tile * 32;
      break;
    case SuperFamicom::PPU::Background::Mode::BPP8:
      tile = y * 32 + x;
      address = tile * 64;
      break;
    case SuperFamicom::PPU::Background::Mode::Mode7:
      tile = y * 16 + x;
      address = tile * 128 + 1;
      break;
    }
    output.append("Tile: 0x", hex(tile, 4L), ", Address: 0x", hex(address, 4L));
    statusBar.setText(output);
  });

  modeChanged();

  setGeometry({{128, 128}, layout.minimumSize()});
}

auto VRAMViewer::modeChanged() -> void {
  paletteSelection.reset();
  switch(modeSelection.selected().offset()) {
  case SuperFamicom::PPU::Background::Mode::BPP2:
    for(uint bg = 0; bg < 4; bg++) {
      for(uint palette = 0; palette < 8; palette++) {
        paletteSelection.append(ComboButtonItem().setText({"BG", bg, " ", palette}));
      }
    }
    break;
  case SuperFamicom::PPU::Background::Mode::BPP4:
    for(uint palette = 0; palette < 8; palette++) {
      paletteSelection.append(ComboButtonItem().setText({"BG ", palette}));
    }
    for(uint palette = 0; palette < 8; palette++) {
      paletteSelection.append(ComboButtonItem().setText({"SP ", palette}));
    }
    break;
  case SuperFamicom::PPU::Background::Mode::BPP8:
  case SuperFamicom::PPU::Background::Mode::Mode7:
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
  const uint16* sp = SuperFamicom::ppu.vram.data;

  switch(modeSelection.selected().offset()) {
  case SuperFamicom::PPU::Background::Mode::BPP2:
    for(uint tileY : range(64)) {
      for(uint tileX : range(64)) {
        for(uint y : range(8)) {
          uint16 d[] = { sp[0] };
          for(uint x : range(8)) {
            uint color = 0;
            color += d[0] & 0x0080 ? 1 : 0;
            color += d[0] & 0x8000 ? 2 : 0;
            for(auto& b : d) b <<= 1;
            color += paletteSelection.selected().offset() << 2;
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp++;
        }
      }
    }
    break;

  case SuperFamicom::PPU::Background::Mode::BPP4:
    for(uint tileY : range(64)) {
      for(uint tileX : range(32)) {
        for(uint y : range(8)) {
          uint16 d[] = { sp[0], sp[8] };
          for(uint x : range(8)) {
            uint color = 0;
            color += d[0] & 0x0080 ? 1 : 0;
            color += d[0] & 0x8000 ? 2 : 0;
            color += d[1] & 0x0080 ? 4 : 0;
            color += d[1] & 0x8000 ? 8 : 0;
            for(auto& b : d) b <<= 1;
            color += paletteSelection.selected().offset() << 4;
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp++;
        }
        sp += 8;
      }
    }
    break;

  case SuperFamicom::PPU::Background::Mode::BPP8:
    for(uint tileY : range(32)) {
      for(uint tileX : range(32)) {
        for(uint y : range(8)) {
          uint16 d[] = { sp[0], sp[8], sp[16], sp[24] };
          for(uint x : range(8)) {
            uint color = 0;
            color += d[0] & 0x0080 ?   1 : 0;
            color += d[0] & 0x8000 ?   2 : 0;
            color += d[1] & 0x0080 ?   4 : 0;
            color += d[1] & 0x8000 ?   8 : 0;
            color += d[2] & 0x0080 ?  16 : 0;
            color += d[2] & 0x8000 ?  32 : 0;
            color += d[3] & 0x0080 ?  64 : 0;
            color += d[3] & 0x8000 ? 128 : 0;
            for(auto& b : d) b <<= 1;
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp++;
        }
        sp += 24;
      }
    }
    break;

  case SuperFamicom::PPU::Background::Mode::Mode7:
    for(uint tileY : range(32)) {
      for(uint tileX : range(16)) {
        for(uint y : range(8)) {
          for(uint x : range(8)) {
            uint color = 0;
            uint16 d = sp[x];
            color += d.byte(1);
            color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 8;
        }
      }
    }
    break;
  }

  canvas.update();
}
