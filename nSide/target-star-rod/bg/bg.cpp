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
  canvas.setSize({1024, 512});

  bgSelection.onChange({ &BGViewer::bgChanged, this });
  update.onActivate({ &BGViewer::updateTiles, this });

  canvas.onMouseLeave([&] { statusBar.setText(""); });
  canvas.onMouseMove([&](Position position) {
    SuperFamicom::PPU::Background* bg;
    switch(bgSelection.selected().offset()) {
    case SuperFamicom::PPU::Background::ID::BG1: bg = &SuperFamicom::ppu.bg1; break;
    case SuperFamicom::PPU::Background::ID::BG2: bg = &SuperFamicom::ppu.bg2; break;
    case SuperFamicom::PPU::Background::ID::BG3: bg = &SuperFamicom::ppu.bg3; break;
    case SuperFamicom::PPU::Background::ID::BG4: bg = &SuperFamicom::ppu.bg4; break;
    }
    uint x = position.x(), y = position.y() + scroll.position() * 8, mode = bg->r.mode;
    uint tile = 0, address = 0;
    if(mode != SuperFamicom::PPU::Background::Mode::Mode7) {
      tile = bg->getTile(x, y) & 0x03ff;
      x /= 8, y /= 8;
      address = bg->r.tiledataAddress + tile * (16 << mode);
    } else {
      x /= 8, y /= 8;
      tile = SuperFamicom::ppu.vram[y * 128 + x].byte(0) & 0x03ff;
      address = tile * 8 * 8 * 2 + 1;
    }
    string output = { x, ", ", y, ", " };
    output.append(
      "Tile: 0x", hex(tile, 4L), ", ",
      "Address: 0x", hex(address, 4L)
    );
    statusBar.setText(output);
  });

  scroll.setLength(1);
  scroll.setPosition(0);
  scroll.onChange({ &BGViewer::updateTiles, this });

  bgChanged();

  setGeometry({{128, 128}, layout.minimumSize()});
}

auto BGViewer::bgChanged() -> void {
  updateTiles();
}

auto BGViewer::updateTiles() -> void {
  uint32_t* dp = canvas.data();
  for(uint y : range(512)) {
    for(uint x : range(1024)) {
      *dp++ = 0xff800000;
    }
  }
  dp = canvas.data();
  uint16 tiledata;
  const uint16* sp = SuperFamicom::ppu.vram.data;
  SuperFamicom::PPU::Background* bg;
  switch(bgSelection.selected().offset()) {
  case 0: bg = &SuperFamicom::ppu.bg1; break;
  case 1: bg = &SuperFamicom::ppu.bg2; break;
  case 2: bg = &SuperFamicom::ppu.bg3; break;
  case 3: bg = &SuperFamicom::ppu.bg4; break;
  }
  uint16 screenAddress = bg->r.screenAddress;
  uint pitch = 8 << bg->r.mode;
  bool hires = SuperFamicom::ppu.r.bgMode == 5 || SuperFamicom::ppu.r.bgMode == 6;
  bool tileSize = bg->r.tileSize;
  bool tileSize_x = tileSize || hires;
  bool tileSize_y = tileSize;

  uint10 tile_id;
  uint3 palette;
  bool priority;
  bool mirror_x;
  bool mirror_y;

  uint canvasTileY = 0;
  if(bg->r.mode != SuperFamicom::PPU::Background::Mode::Mode7) {
    scroll.setLength((tileSize_y & bg->r.screenSize.bit(1)) * 64 + 1);
    for(uint tileY : range(scroll.position(), scroll.position() + (32 << (tileSize_y | bg->r.screenSize.bit(1))))) {
      canvasTileY = tileY - scroll.position();
      for(uint tileX : range(32 << (tileSize_x + bg->r.screenSize.bit(0)))) {
        tiledata = bg->getTile(tileX * 8, tileY * 8);
        tile_id  = tiledata.bits( 9,  0);
        palette  = tiledata.bits(12, 10);
        priority = tiledata.bit(13);
        mirror_x = tiledata.bit(14);
        mirror_y = tiledata.bit(15);
        if(tileSize_x && (tileX & 1) ^ mirror_x) tile_id += 1;
        if(tileSize_y && (tileY & 1) ^ mirror_y) tile_id += 16;
        sp = SuperFamicom::ppu.vram.data + (bg->r.tiledataAddress + tile_id * pitch);
        switch(bg->r.mode) {
        case SuperFamicom::PPU::Background::Mode::BPP2:
          for(uint y : (!mirror_y ? range(8) : rrange(8))) {
            uint16 d[] = { sp[0] };
            for(uint x : (!mirror_x ? range(8) : rrange(8))) {
              uint color = 0;
              color += d[0] & 0x0080 ? 1 : 0;
              color += d[0] & 0x8000 ? 2 : 0;
              for(auto& b : d) b <<= 1;
              color += palette << 2;
              color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
              color = (255u << 24) |
                (image::normalize(color >>  0 & 31, 5, 8) << 16) |
                (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
                (image::normalize(color >> 10 & 31, 5, 8) <<  0);
              dp[(canvasTileY * 8 + y) * 1024 + (tileX * 8 + x)] = color;
            }
            sp++;
          }
          break;

        case SuperFamicom::PPU::Background::Mode::BPP4:
          for(uint y : (!mirror_y ? range(8) : rrange(8))) {
            uint16 d[] = { sp[0], sp[8] };
            for(uint x : (!mirror_x ? range(8) : rrange(8))) {
              uint color = 0;
              color += d[0] & 0x0080 ? 1 : 0;
              color += d[0] & 0x8000 ? 2 : 0;
              color += d[1] & 0x0080 ? 4 : 0;
              color += d[1] & 0x8000 ? 8 : 0;
              for(auto& b : d) b <<= 1;
              color += palette << 4;
              color = SuperFamicom::ppu.cgram[color << 1] | SuperFamicom::ppu.cgram[color << 1 | 1] << 8;
              color = (255u << 24) |
                (image::normalize(color >>  0 & 31, 5, 8) << 16) |
                (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
                (image::normalize(color >> 10 & 31, 5, 8) <<  0);
              dp[(canvasTileY * 8 + y) * 1024 + (tileX * 8 + x)] = color;
            }
            sp++;
          }
          break;

        case SuperFamicom::PPU::Background::Mode::BPP8:
          for(uint y : (!mirror_y ? range(8) : rrange(8))) {
            uint16 d[] = { sp[0], sp[8], sp[16], sp[24] };
            for(uint x : (!mirror_x ? range(8) : rrange(8))) {
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
              dp[(canvasTileY * 8 + y) * 1024 + (tileX * 8 + x)] = color;
            }
            sp++;
          }
          break;

        }
      }
    }
  } else {
    scroll.setLength(64 + 1);
    for(uint tileY : range(scroll.position(), scroll.position() + 64)) {
      canvasTileY = tileY - scroll.position();
      for(uint tileX : range(128)) {
        tile_id = SuperFamicom::ppu.vram[tileY * 128 + tileX].byte(0);
        sp = SuperFamicom::ppu.vram.data + (tile_id * 8 * 8);
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
            dp[(canvasTileY * 8 + y) * 1024 + (tileX * 8 + x)] = color;
          }
          sp += 8;
        }
      }
    }
  }

  canvas.update();
}
