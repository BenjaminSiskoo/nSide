auto PPU::Background::power() -> void {
  io.tiledataAddress = (random() & 0x0f) << 12;
  io.screenAddress = (random() & 0xfc) << 8;
  io.screenSize = random();
  io.mosaic = 0;
  io.tileSize = random();
  io.mode = 0;
  for(auto& p : io.priority) p = 0;
  io.aboveEnable = random();
  io.belowEnable = random();
  io.hoffset = random();
  io.voffset = random();

  //x = 0;
  y = 0;

  tile = 0;
  priority = 0;
  paletteNumber = 0;
  paletteIndex = 0;
}

auto PPU::Background::getTile(uint x, uint y) -> uint16 {
  x = (x & maskX) >> tileWidth;
  y = (y & maskY) >> tileHeight;

  uint16 offset = ((y & 0x1f) << 5) + (x & 0x1f);
  if(x & 0x20) offset += screenX;
  if(y & 0x20) offset += screenY;

  const uint16 address = io.screenAddress + offset;
  return ppu.vram[address];
}

//called once at the start of every rendered scanline
auto PPU::Background::scanline() -> void {
  const uint hires = ppu.io.bgMode == 5 || ppu.io.bgMode == 6;
  const uint width = !hires ? 256 : 512;

  tileHeight = io.tileSize ? 4 : 3;
  tileWidth = hires ? 4 : uint(tileHeight);

  maskX = tileHeight == 4 ? width << 1 : width;
  maskY = maskX;
  if(io.screenSize & 0x01) maskX <<= 1;
  if(io.screenSize & 0x02) maskY <<= 1;
  maskX--;
  maskY--;

  screenX = io.screenSize & 0x01 ? 32 << 5 : 0;
  screenY = io.screenSize & 0x02 ? 32 << 5 : 0;
  if(io.screenSize == ScreenSize::Size64x64) screenY <<= 1;
}

auto PPU::Background::directColor(uint8 palette, uint16 tile) const -> uint16 {
  //palette = -------- BBGGGRRR
  //tile    = ---bgr-- --------
  //output  = 0BBb00GG Gg0RRRr0
  return ((palette << 7) & 0x6000) + ((tile >> 0) & 0x1000)
       + ((palette << 4) & 0x0380) + ((tile >> 5) & 0x0040)
       + ((palette << 2) & 0x001c) + ((tile >> 9) & 0x0002);
}

auto PPU::Background::render() -> void {
  if(io.mode == Mode::Inactive) return;
  if(io.priority[0] + io.priority[1] == 0) return;
  if(!io.aboveEnable && !io.belowEnable) return;

  if(io.mode == Mode::Mode7) return renderMode7();

  const uint16 validMask = id == ID::BG1 ? 0x2000 : 0x4000;

  const uint paletteOffset = ppu.io.bgMode == 0 ? id << 5 : 0;
  const uint paletteSize = 2 << io.mode;
  const uint tileMask = ppu.vram.mask >> (3 + io.mode);
  const uint tiledataIndex = io.tiledataAddress >> (3 + io.mode);

//const uint8* bg_tiledata  = ppu.cache.tiledata[io.mode];
//const uint8* bg_tilevalid = ppu.cache.tilevalid[io.mode];

  y = ppu.io.bg_y[id];
  uint16 hscroll = io.hoffset;
  uint16 vscroll = io.voffset;

  const uint hires = ppu.io.bgMode == 5 || ppu.io.bgMode == 6;
  const uint width = 256 << hires;

  if(hires) {
    hscroll <<= 1;
    if(ppu.io.interlace) y = (y << 1) + ppu.field();
  }

  uint16 hval, vval;
  uint16 hoffset, voffset, offsetX, col;
  bool   mirrorX, mirrorY;

  const uint8*  tile_ptr;
  const uint16* mtable = ppu.mosaicTable[io.mosaic];
  const bool isOPTMode = ppu.io.bgMode == 2 || ppu.io.bgMode == 4 || ppu.io.bgMode == 6;
  const bool isDirectColorMode = ppu.screen.io.directColor && id == ID::BG1 && (ppu.io.bgMode == 3 || ppu.io.bgMode == 4);

  ppu.window.buildTables(id);
  const uint8* wt_above = ppu.window.cache[id].above;
  const uint8* wt_below = ppu.window.cache[id].below;

  uint16 prev_x = 0xffff, prev_y = 0xffff, prev_optx = 0xffff;
  for(uint16 x : range(width)) {
    hoffset = mtable[x] + hscroll;
    voffset = y + vscroll;

    if(isOPTMode) {
      offsetX = (x + (hscroll & 7));

      //tile 0 is unaffected by offset-per-tile mode...
      if(offsetX >= 8) {

        //cache tile data in hval, vval if possible
        if((offsetX >> 3) != (prev_optx >> 3)) {
          prev_optx = offsetX;

          hval = ppu.bg3.getTile((offsetX - 8) + (ppu.bg3.io.hoffset & ~7), ppu.bg3.io.voffset);
          if(ppu.io.bgMode != 4) {
            vval = ppu.bg3.getTile((offsetX - 8) + (ppu.bg3.io.hoffset & ~7), ppu.bg3.io.voffset + 8);
          }
        }

        if(ppu.io.bgMode == 4) {
          if(hval & validMask) {
            if(!(hval & 0x8000)) {
              hoffset = offsetX + (hval & ~7);
            } else {
              voffset = y + hval;
            }
          }
        } else {
          if(hval & validMask) {
            hoffset = offsetX + (hval & ~7);
          }
          if(vval & validMask) {
            voffset = y + vval;
          }
        }
      }
    }

    hoffset &= maskX;
    voffset &= maskY;

    if(hoffset >> 3 != prev_x || voffset >> 3 != prev_y) {
      prev_x = hoffset >> 3;
      prev_y = voffset >> 3;

      tile = getTile((uint)hoffset, (uint)voffset);  //format = vhopppcc cccccccc
      mirrorY = tile & 0x8000;
      mirrorX = tile & 0x4000;
      priority = io.priority[(tile & 0x2000) >> 13];
      paletteNumber = (tile >> 10) & 7;
      paletteIndex = paletteOffset + (paletteNumber << paletteSize);

      if(tileWidth  == 4) {  //16x16 horizontal tile mirroring
        if((bool)(hoffset & 8) != mirrorX) tile++;
      }

      if(tileHeight == 4) {  //16x16 vertical tile mirroring
        if((bool)(voffset & 8) != mirrorY) tile += 16;
      }

      uint16 character = ((tile & 0x03ff) + tiledataIndex) & tileMask;

      //if(bg_tilevalid[character] == 0) {
      //  ppu.cache.tile(io.mode, character);
      //}

      if(mirrorY) voffset ^= 7;  //invert y tile pos
      tile_ptr = ppu.cache.tile(io.mode, character) + ((voffset & 7) << 3);
    }

    if(mirrorX) hoffset ^= 7;  //invert x tile pos
    col = *(tile_ptr + (hoffset & 7));
    if(col) {
      if(isDirectColorMode) {
        col = directColor(col, tile);
      } else {
        col = ppu.screen.cgram[col | paletteIndex];
      }

      if(!hires) {
        if(io.aboveEnable && !wt_above[x]) ppu.screen.output.plotAbove(x, col, priority, id, false);
        if(io.belowEnable && !wt_below[x]) ppu.screen.output.plotBelow(x, col, priority, id, false);
      } else {
        int hx = x >> 1;
        if(x & 1) {
          if(io.aboveEnable && !wt_above[hx]) ppu.screen.output.plotAbove(hx, col, priority, id, false);
        } else {
          if(io.belowEnable && !wt_below[hx]) ppu.screen.output.plotBelow(hx, col, priority, id, false);
        }
      }
    }
  }
}

//higan mode7 renderer
//
//base algorithm written by anomie
//higan implementation written by byuu
//
//supports mode 7 + extbg + rotate + zoom + direct color + scrolling + m7sel + windowing + mosaic
//interlace and pseudo-hires support are automatic via main rendering routine

auto PPU::Background::renderMode7() -> void {
  int32 px, py;
  int32 tx, ty, tile, palette;

  int32 a = sclip<16>(m7cache.a);
  int32 b = sclip<16>(m7cache.b);
  int32 c = sclip<16>(m7cache.c);
  int32 d = sclip<16>(m7cache.d);

  int32 cx   = sclip<13>(m7cache.x);
  int32 cy   = sclip<13>(m7cache.y);
  int32 hofs = sclip<13>(m7cache.hoffset);
  int32 vofs = sclip<13>(m7cache.voffset);

  int _pri, _x;

  ppu.window.buildTables(id);
  uint8* wt_above = ppu.window.cache[id].above;
  uint8* wt_below = ppu.window.cache[id].below;

  y = !ppu.io.vflipMode7 ? ppu.vcounter() : uint16(255 - ppu.vcounter());

  uint16* mtable_x;
  uint16* mtable_y;
  if(id == ID::BG1) {
    mtable_x = (uint16*)ppu.mosaicTable[ppu.bg1.io.mosaic];
    mtable_y = (uint16*)ppu.mosaicTable[ppu.bg1.io.mosaic];
  } else {  //id == ID::BG2
    //Mode7 EXTBG BG2 uses BG1 mosaic enable to control vertical mosaic,
    //and BG2 mosaic enable to control horizontal mosaic...
    mtable_x = (uint16*)ppu.mosaicTable[ppu.bg2.io.mosaic];
    mtable_y = (uint16*)ppu.mosaicTable[ppu.bg1.io.mosaic];
  }

  //13-bit sign extend
  //--s---vvvvvvvvvv -> ssssssvvvvvvvvvv
  #define clip(x) (x & 0x2000 ? x | ~0x03ff : x & 0x03ff)
  int32 psx = ((a * clip(hofs - cx)) & ~63) + ((b * clip(vofs - cy)) & ~63) + ((b * mtable_y[y]) & ~63) + (cx << 8);
  int32 psy = ((c * clip(hofs - cx)) & ~63) + ((d * clip(vofs - cy)) & ~63) + ((d * mtable_y[y]) & ~63) + (cy << 8);
  #undef clip

  for(int32 x = 0; x < 256; x++) {
    px = psx + (a * mtable_x[x]);
    py = psy + (c * mtable_x[x]);

    //mask floating-point bits (low 8 bits)
    px >>= 8;
    py >>= 8;

    switch(ppu.io.repeatMode7) {
    //screen repetition outside of screen area
    case 0:
    case 1:
      px &= 1023;
      py &= 1023;
      tile = ppu.vram[(py >> 3) * 128 + (px >> 3)].byte(0);
      palette = ppu.vram[(tile << 6) + ((py & 7) << 3) + (px & 7)].byte(1);
      break;

    //palette color 0 outside of screen area
    case 2: 
      if((px | py) & ~1023) {
        palette = 0;
      } else {
        px &= 1023;
        py &= 1023;
        tile = ppu.vram[(py >> 3) * 128 + (px >> 3)].byte(0);
        palette = ppu.vram[(tile << 6) + ((py & 7) << 3) + (px & 7)].byte(1);
      }
      break;

    //character 0 repetition outside of screen area
    case 3:
      if((px | py) & ~1023) {
        tile = 0;
      } else {
        px &= 1023;
        py &= 1023;
        tile = ppu.vram[(py >> 3) * 128 + (px >> 3)].byte(0);
      }
      palette = ppu.vram[(tile << 6) + ((py & 7) << 3) + (px & 7)].byte(1);
      break;

    }

    if(id == ID::BG1) {
      _pri = io.priority[0];
    } else {
      _pri = io.priority[(palette >> 7) ? 1 : 0];
      palette &= 0x7f;
    }

    if(!palette) continue;

    _x = !ppu.io.hflipMode7 ? (uint)x : 255 - x;

    uint16 color;
    if(ppu.screen.io.directColor && id == ID::BG1) {
      //direct color mode does not apply to bg2, as it is only 128 colors...
      color = directColor(palette, 0);
    } else {
      color = ppu.screen.cgram[palette];
    }

    if(io.aboveEnable && !wt_above[_x]) {
      ppu.screen.output.plotAbove(_x, color, _pri, id, false);
    }
    if(io.belowEnable && !wt_below[_x]) {
      ppu.screen.output.plotBelow(_x, color, _pri, id, false);
    }
  }
}
