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
    SFC::PPU::Background* bg;
    switch(bgSelection.selected().offset()) {
    case SFC::PPU::Background::ID::BG1: bg = &SFC::ppu.bg1; break;
    case SFC::PPU::Background::ID::BG2: bg = &SFC::ppu.bg2; break;
    case SFC::PPU::Background::ID::BG3: bg = &SFC::ppu.bg3; break;
    case SFC::PPU::Background::ID::BG4: bg = &SFC::ppu.bg4; break;
    }
    uint x = position.x(), y = position.y() + scroll.position() * 8, mode = bg->io.mode;
    uint tile = 0, dataAddress = 0, nameAddress = 0;
    if(mode != SFC::PPU::Background::Mode::Mode7) {
      tile = bg->getTile(x, y) & 0x03ff;

      bool hires = (SFC::ppu.io.bgMode == 5 || SFC::ppu.io.bgMode == 6);
      uint tileHeight = (bg->io.tileSize == SFC::PPU::Background::TileSize::Size8x8 ? 3 : 4);
      uint tileWidth = (!hires ? tileHeight : 4);
      uint width = (!hires ? 256 : 512);
      uint maskX = (tileHeight == 3 ? width : width << 1);
      uint maskY = maskX;
      if(bg->io.screenSize & 1) maskX <<= 1;
      if(bg->io.screenSize & 2) maskY <<= 1;
      maskX--;
      maskY--;

      uint screenX = (bg->io.screenSize & 1 ? 32 << 5 : 0);
      uint screenY = (bg->io.screenSize & 2 ? 32 << 5 : 0);
      if(bg->io.screenSize == 3) screenY <<= 1;

      x = (x & maskX) >> tileWidth;
      y = (y & maskY) >> tileHeight;

      uint16 offset = ((y & 0x1f) << 5) + (x & 0x1f);
      if(x & 0x20) offset += screenX;
      if(y & 0x20) offset += screenY;

      nameAddress = (bg->io.screenAddress + offset) << 1;
      dataAddress = (bg->io.tiledataAddress << 1) + tile * (16 << mode);
    } else {
      x >>= 3, y >>= 3;
      tile = SFC::ppu.vram[y * 128 + x].byte(0) & 0x03ff;
      nameAddress = (((y & 0x7f) * 128 + (x & 0x7f)) << 1) + 0;
      dataAddress = ((tile * 8 * 8) << 1) + 1;
    }
    string output = { x, ", ", y, ", " };
    output.append(
      "Tile: 0x", hex(tile, 4L), ", ",
      "Data Address: 0x", hex(dataAddress, 4L), ", ",
      "Name Address: 0x", hex(nameAddress, 4L)
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
  for(uint y : range(512))
  for(uint x : range(1024)) {
    *dp++ = 0xff800000;
  }
  dp = canvas.data();
  uint16 tiledata;
  const uint16* sp = SFC::ppu.vram.data;
  SFC::PPU::Background* bg;
  switch(bgSelection.selected().offset()) {
  case 0: bg = &SFC::ppu.bg1; break;
  case 1: bg = &SFC::ppu.bg2; break;
  case 2: bg = &SFC::ppu.bg3; break;
  case 3: bg = &SFC::ppu.bg4; break;
  }
  uint16 screenAddress = bg->io.screenAddress;
  uint pitch = 8 << bg->io.mode;
  bool hires = SFC::ppu.io.bgMode == 5 || SFC::ppu.io.bgMode == 6;
  bool tileHeight = bg->io.tileSize;
  bool tileWidth  = tileHeight || hires;

  uint10 tileID;
  uint3 palette;
  bool priority;
  bool mirrorX;
  bool mirrorY;
  uint color;

  uint canvasTileY = 0;
  if(bg->io.mode != SFC::PPU::Background::Mode::Mode7) {
    scroll.setLength((tileHeight & bg->io.screenSize.bit(1)) * 64 + 1);
    for(uint tileY : range(scroll.position(), scroll.position() + (32 << (tileHeight | bg->io.screenSize.bit(1))))) {
      canvasTileY = tileY - scroll.position();
      for(uint tileX : range(32 << (tileWidth + bg->io.screenSize.bit(0)))) {
        tiledata = bg->getTile(tileX * 8, tileY * 8);
        tileID   = tiledata.bits( 9,  0);
        palette  = tiledata.bits(12, 10);
        priority = tiledata.bit(13);
        mirrorX  = tiledata.bit(14);
        mirrorY = tiledata.bit(15);
        if(tileWidth  && (tileX & 1) ^ mirrorX) tileID +=  1;
        if(tileHeight && (tileY & 1) ^ mirrorY) tileID += 16;
        sp = SFC::ppu.vram.data + (bg->io.tiledataAddress + tileID * pitch);
        switch(bg->io.mode) {
        case SFC::PPU::Background::Mode::BPP2:
          for(uint y : (!mirrorY ? range(8) : rrange(8))) {
            uint16 d[] = { sp[0] };
            for(uint x : (!mirrorX ? range(8) : rrange(8))) {
              color = 0;
              color += d[0] & 0x0080 ? 1 : 0;
              color += d[0] & 0x8000 ? 2 : 0;
              for(auto& b : d) b <<= 1;
              color += palette << 2;
              color = SFC::ppu.screen.cgram[color];
              color = (255u << 24) |
                (image::normalize(color >>  0 & 31, 5, 8) << 16) |
                (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
                (image::normalize(color >> 10 & 31, 5, 8) <<  0);
              dp[(canvasTileY * 8 + y) * 1024 + (tileX * 8 + x)] = color;
            }
            sp++;
          }
          break;

        case SFC::PPU::Background::Mode::BPP4:
          for(uint y : (!mirrorY ? range(8) : rrange(8))) {
            uint16 d[] = { sp[0], sp[8] };
            for(uint x : (!mirrorX ? range(8) : rrange(8))) {
              color = 0;
              color += d[0] & 0x0080 ? 1 : 0;
              color += d[0] & 0x8000 ? 2 : 0;
              color += d[1] & 0x0080 ? 4 : 0;
              color += d[1] & 0x8000 ? 8 : 0;
              for(auto& b : d) b <<= 1;
              color += palette << 4;
              color = SFC::ppu.screen.cgram[color];
              color = (255u << 24) |
                (image::normalize(color >>  0 & 31, 5, 8) << 16) |
                (image::normalize(color >>  5 & 31, 5, 8) <<  8) |
                (image::normalize(color >> 10 & 31, 5, 8) <<  0);
              dp[(canvasTileY * 8 + y) * 1024 + (tileX * 8 + x)] = color;
            }
            sp++;
          }
          break;

        case SFC::PPU::Background::Mode::BPP8:
          for(uint y : (!mirrorY ? range(8) : rrange(8))) {
            uint16 d[] = { sp[0], sp[8], sp[16], sp[24] };
            for(uint x : (!mirrorX ? range(8) : rrange(8))) {
              color = 0;
              color += d[0] & 0x0080 ?   1 : 0;
              color += d[0] & 0x8000 ?   2 : 0;
              color += d[1] & 0x0080 ?   4 : 0;
              color += d[1] & 0x8000 ?   8 : 0;
              color += d[2] & 0x0080 ?  16 : 0;
              color += d[2] & 0x8000 ?  32 : 0;
              color += d[3] & 0x0080 ?  64 : 0;
              color += d[3] & 0x8000 ? 128 : 0;
              for(auto& b : d) b <<= 1;
              color = SFC::ppu.screen.cgram[color];
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
        tileID = SFC::ppu.vram[tileY * 128 + tileX].byte(0);
        sp = SFC::ppu.vram.data + (tileID * 8 * 8);
        for(uint y : range(8)) {
          for(uint x : range(8)) {
            color = SFC::ppu.screen.cgram[sp[x] >> 8];
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
