#include "../laevateinn.hpp"
unique_pointer<BGViewer> bgViewer;

BGViewer::BGViewer() {
  bgViewer = this;
  setTitle("BG Viewer");
  statusBar.setFont(Font().setBold());
  statusBar.setVisible();

  layout.setMargin(5);
  bgLabel.setText("Mode:");
  bgSelection.append(ComboButtonItem().setText("BG1"));
  bgSelection.append(ComboButtonItem().setText("BG2"));
  bgSelection.append(ComboButtonItem().setText("BG3"));
  bgSelection.append(ComboButtonItem().setText("BG4"));
  autoUpdate.setText("Auto");
  update.setText("Update");
  canvas.setSize({512, 512});

  bgSelection.onChange({ &BGViewer::bgChanged, this });
  update.onActivate({ &BGViewer::updateTiles, this });

  canvas.onMouseLeave([&] { statusBar.setText(""); });
  canvas.onMouseMove([&](Position position) {
    SuperFamicom::PPU::Background* bg;
    switch(bgSelection.selected().offset()) {
    case 0: bg = &SuperFamicom::ppu.bg1; break;
    case 1: bg = &SuperFamicom::ppu.bg2; break;
    case 2: bg = &SuperFamicom::ppu.bg3; break;
    case 3: bg = &SuperFamicom::ppu.bg4; break;
    }
    uint x = position.x(), y = position.y(), mode = bg->r.mode;
    uint16_t tiledata = bg->getTile(x, y);
    x /= 8, y /= 8;
    string output = { x, ", ", y, ", " };
    uint tile = tiledata & 0x03ff;
    output.append(
      "Tile: 0x", hex(tile, 4L), ", ",
      "Address: 0x", hex(bg->r.tiledataAddress + tile * (16 << mode), 4L)
    );
    statusBar.setText(output);
  });

  bgChanged();

  setGeometry({{128, 128}, layout.minimumSize()});
  windowManager->append(this, "BGViewer");
}

auto BGViewer::bgChanged() -> void {
  updateTiles();
}

auto BGViewer::updateTiles() -> void {
  uint32_t* dp = canvas.data();
  for(uint y = 0; y < 512; y++) {
    for(uint x = 0; x < 512; x++) {
      *dp++ = 0xff800000;
    }
  }
  dp = canvas.data();
  uint16 tiledata;
  const uint8* sp = SuperFamicom::ppu.memory.vram;
  SuperFamicom::PPU::Background* bg;
  switch(bgSelection.selected().offset()) {
  case 0: bg = &SuperFamicom::ppu.bg1; break;
  case 1: bg = &SuperFamicom::ppu.bg2; break;
  case 2: bg = &SuperFamicom::ppu.bg3; break;
  case 3: bg = &SuperFamicom::ppu.bg4; break;
  }
  uint16 screenAddress = bg->r.screenAddress;
  uint pitch = 16 << bg->r.mode;
  bool hires = SuperFamicom::ppu.r.bgMode == 5 || SuperFamicom::ppu.r.bgMode == 6;
  bool tileSize = bg->r.tileSize;
  bool tileSize_x = tileSize || hires;
  bool tileSize_y = tileSize;

  uint10 tile_id;
  uint3 palette;
  bool priority;
  bool mirror_x;
  bool mirror_y;

  for(uint tileY : range(tileSize_y || (bg->r.screenSize & 2) ? 64 : 32)) {
    for(uint tileX : range(tileSize_x || (bg->r.screenSize & 1) ? 64 : 32)) {
      tiledata = bg->getTile(tileX * 8, tileY * 8);
      tile_id  = tiledata.bits( 9,  0);
      palette  = tiledata.bits(12, 10);
      priority = tiledata.bit(13);
      mirror_x = tiledata.bit(14);
      mirror_y = tiledata.bit(15);
      if(tileSize_x && (tileX & 1) ^ mirror_x) tile_id += 1;
      if(tileSize_y && (tileY & 1) ^ mirror_y) tile_id += 16;
      sp = SuperFamicom::ppu.memory.vram + (bg->r.tiledataAddress + tile_id * pitch);
      switch(bg->r.mode) {
      case SuperFamicom::PPU::Background::Mode::BPP2:
        for(uint y : (!mirror_y ? range(8) : rrange(8))) {
          uint8 d[] = { sp[0], sp[1] };
          for(uint x : (!mirror_x ? range(8) : rrange(8))) {
            uint color = 0;
            color += d[0] & 0x80 ? 1 : 0;
            color += d[1] & 0x80 ? 2 : 0;
            for(auto& b : d) b <<= 1;
            color += palette << 2;
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        break;
      case SuperFamicom::PPU::Background::Mode::BPP4:
        for(uint y : (!mirror_y ? range(8) : rrange(8))) {
          uint8 d[] = { sp[0], sp[1], sp[16], sp[17] };
          for(uint x : (!mirror_x ? range(8) : rrange(8))) {
            uint color = 0;
            color += d[0] & 0x80 ? 1 : 0;
            color += d[1] & 0x80 ? 2 : 0;
            color += d[2] & 0x80 ? 4 : 0;
            color += d[3] & 0x80 ? 8 : 0;
            for(auto& b : d) b <<= 1;
            color += palette << 4;
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        break;
      case SuperFamicom::PPU::Background::Mode::BPP8:
        for(uint y : (!mirror_y ? range(8) : rrange(8))) {
          uint8 d[] = { sp[0], sp[1], sp[16], sp[17], sp[32], sp[33], sp[48], sp[49] };
          for(uint x : (!mirror_x ? range(8) : rrange(8))) {
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
            color += palette << 4;
            color = SuperFamicom::ppu.memory.cgram[color << 1] | SuperFamicom::ppu.memory.cgram[color << 1 | 1] << 8;
            color = (255u << 24) |
              (image::normalize(color >>  0 & 31, 5, 8) << 16) |
              (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
              (image::normalize(color >> 10 & 31, 5, 8) <<  0);
            dp[(tileY * 8 + y) * 512 + (tileX * 8 + x)] = color;
          }
          sp += 2;
        }
        break;
      }
    }
  }

  canvas.update();
}
