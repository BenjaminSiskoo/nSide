#include "../laevateinn.hpp"
unique_pointer<VRAMViewer> vramViewer;

VRAMViewer::VRAMViewer() {
  vramViewer = this;

  layout.setMargin(5);
  setTitle("VRAM Viewer");
  statusBar.setFont(Font().setBold());
  statusBar.setVisible();

  modeLabel.setText("Mode:");
  modeSelection.append(ComboButtonItem().setText("2BPP"));
  modeSelection.append(ComboButtonItem().setText("4BPP"));
  modeSelection.append(ComboButtonItem().setText("8BPP"));
  modeSelection.append(ComboButtonItem().setText("Mode 7"));
  modeSelection.onChange({ &VRAMViewer::modeChanged, this });
  paletteLabel.setText("Palette:");
  paletteSelection.onChange({ &VRAMViewer::paletteChanged, this });
  autoUpdate.setText("Auto");
  update.setText("Update").onActivate({ &VRAMViewer::updateTiles, this });

  columnLinear.setText("Linear Tiles").onActivate({ &VRAMViewer::columnChanged, this });
  column16.setText("16-tile Columns").onActivate({ &VRAMViewer::columnChanged, this });

  canvas.setSize({512, 512});

  canvas.onMouseLeave([&] { statusBar.setText(""); });
  canvas.onMouseMove([&](Position position) {
    uint x = position.x(), y = position.y();
    uint mode = modeSelection.selected().offset();

    uint columnCount;
    uint columnWidth;
    uint columnHeight;
    if(column16.checked()) {
      columnWidth  = 16;
      columnHeight = min(64, 256 >> mode);
      columnCount  = (4096 >> mode) / (columnWidth * columnHeight);
    } else {
      columnCount  = 1;
      columnWidth  = 64;
      columnHeight = 64 >> mode;
    }

    x >>= 3, y >>= 3;
    if((x >= columnCount * columnWidth)
    || (y >= columnHeight)) { statusBar.setText(""); return; }
    string output = { x, ", ", y, ", " };
    uint tile = 0, address = 0;

    tile = x / columnWidth * columnWidth * columnHeight + y * columnWidth + x % columnWidth;
    address = tile * (16 << mode) + (mode == SFC::PPU::Background::Mode::Mode7);
    output.append("Tile: 0x", hex(tile, 4L), ", Address: 0x", hex(address, 4L));
    statusBar.setText(output);
  });

  modeChanged();

  setSize(layout.minimumSize());
  setAlignment({1.0, 0.0});
}

auto VRAMViewer::modeChanged() -> void {
  paletteSelection.reset();
  switch(modeSelection.selected().offset()) {
  case SFC::PPU::Background::Mode::BPP2:
    for(uint bg : range(4))
    for(uint palette : range(8)) {
      paletteSelection.append(ComboButtonItem().setText({"BG", bg, " ", palette}));
    }
    break;
  case SFC::PPU::Background::Mode::BPP4:
    for(uint palette : range(8)) {
      paletteSelection.append(ComboButtonItem().setText({"BG ", palette}));
    }
    for(uint palette : range(8)) {
      paletteSelection.append(ComboButtonItem().setText({"SP ", palette}));
    }
    break;
  case SFC::PPU::Background::Mode::BPP8:
  case SFC::PPU::Background::Mode::Mode7:
    paletteSelection.append(ComboButtonItem().setText("BG"));
    break;
  }
  paletteChanged();
}

auto VRAMViewer::paletteChanged() -> void {
  updateTiles();
}

auto VRAMViewer::columnChanged() -> void {
  updateTiles();
}

auto VRAMViewer::updateTiles() -> void {
  uint32_t* dp = canvas.data();
  for(uint x : range(canvas.geometry().width() * canvas.geometry().height())) {
    *dp++ = 0xff800000;
  }
  dp = canvas.data();
  const uint16* sp = SFC::ppu.vram.data;

  uint mode = modeSelection.selected().offset();

  uint columnCount;
  uint columnWidth;
  uint columnHeight;
  if(column16.checked()) {
    columnWidth  = 16;
    columnHeight = min(64, 256 >> mode);
    columnCount  = (4096 >> mode) / (columnWidth * columnHeight);
  } else {
    columnCount  = 1;
    columnWidth  = 64;
    columnHeight = 64 >> mode;
  }

  uint color;
  switch(mode) {
  case SFC::PPU::Background::Mode::BPP2:
  case SFC::PPU::Background::Mode::BPP4:
  case SFC::PPU::Background::Mode::BPP8:
    for(uint column : range(columnCount))
    for(uint tileY : range(columnHeight))
    for(uint tileX : range(columnWidth)) {
      for(uint y : range(8)) {
        uint16 d[] = { sp[0], sp[8], sp[16], sp[24] };
        for(uint x : range(8)) {
          color = 0;
          for(uint p : range(1 << mode)) {
            color += d[p] & 0x0080 ? 1 << (p << 1) : 0;
            color += d[p] & 0x8000 ? 2 << (p << 1) : 0;
          }
          for(auto& b : d) b <<= 1;
          color += paletteSelection.selected().offset() << (2 << mode);
          color = SFC::ppu.screen.cgram[color];
          color = (255u << 24) |
            (image::normalize(color >>  0 & 31, 5, 8) << 16) |
            (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
            (image::normalize(color >> 10 & 31, 5, 8) <<  0);
          dp[(column * columnWidth * 8) + (tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
        }
        sp++;
      }
      sp += (8 << mode) - 8;
    }
    break;

  case SFC::PPU::Background::Mode::Mode7:
    for(uint column : range(columnCount))
    for(uint tileY : range(columnHeight))
    for(uint tileX : range(columnWidth)) {
      for(uint y : range(8)) {
        for(uint x : range(8)) {
          color = SFC::ppu.screen.cgram[sp[x] >> 8];
          color = (255u << 24) |
            (image::normalize(color >>  0 & 31, 5, 8) << 16) |
            (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
            (image::normalize(color >> 10 & 31, 5, 8) <<  0);
          dp[(column * columnWidth * 8) + (tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
        }
        sp += 8;
      }
    }
    break;
  }

  canvas.update();
}
