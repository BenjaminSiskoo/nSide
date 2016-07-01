//called once at the start of every rendered scanline
auto PPU::updateBGInfo() -> void {
  const uint hires = io.bgMode == 5 || io.bgMode == 6;
  const uint width = !hires ? 256 : 512;

  auto bg = &bg1;
  for(uint bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) {
    switch(bg_id) {
    case Background::ID::BG1: bg = &bg1; break;
    case Background::ID::BG2: bg = &bg2; break;
    case Background::ID::BG3: bg = &bg3; break;
    case Background::ID::BG4: bg = &bg4; break;
    }

    bg_info[bg_id].tileHeight = bg->io.tileSize ? 4 : 3;
    bg_info[bg_id].tileWidth = hires ? 4 : (uint)bg_info[bg_id].tileHeight;

    bg_info[bg_id].maskX = bg_info[bg_id].tileHeight == 4 ? width << 1 : width;
    bg_info[bg_id].maskY = bg_info[bg_id].maskX;
    if(bg->io.screenSize & 0x01) bg_info[bg_id].maskX <<= 1;
    if(bg->io.screenSize & 0x02) bg_info[bg_id].maskY <<= 1;
    bg_info[bg_id].maskX--;
    bg_info[bg_id].maskY--;

    bg_info[bg_id].screenX = bg->io.screenSize & 0x01 ? 32 << 5 : 0;
    bg_info[bg_id].screenY = bg->io.screenSize & 0x02 ? 32 << 5 : 0;
    if(bg->io.screenSize == Background::ScreenSize::Size64x64) bg_info[bg_id].screenY <<= 1;
  }
}

template<uint bg_id>
auto PPU::bg_renderLine() -> void {
  if(io.bgMode == 7) return bg_renderLineMode7<bg_id>();
  Background& bg = (
  bg_id == Background::ID::BG1 ? bg1 :
  bg_id == Background::ID::BG2 ? bg2 :
  bg_id == Background::ID::BG3 ? bg3 :
                                 bg4
  );
  if(bg.io.mode == Background::Mode::Inactive) return;
  if(bg.io.priority[0] + bg.io.priority[1] == 0) return;
  if(!bg.io.aboveEnable && !bg.io.belowEnable) return;

  const uint16 validMask = bg_id == Background::ID::BG1 ? 0x2000 : 0x4000;

  const uint paletteOffset = (io.bgMode == 0 ? bg_id << 5 : 0);
  const uint paletteSize = 2 << bg.io.mode;
  const uint tileMask = ppu.vram.mask >> (3 + bg.io.mode);
  const uint tiledataIndex = bg.io.tiledataAddress >> (3 + bg.io.mode);

  const uint8* bg_tiledata  = tiledataCache.tiledata[bg.io.mode];
  const uint8* bg_tilestate = tiledataCache.tiledataState[bg.io.mode];

  const uint8  tileWidth  = bg_info[bg_id].tileWidth;
  const uint8  tileHeight = bg_info[bg_id].tileHeight;
  const uint16 maskX      = bg_info[bg_id].maskX;
  const uint16 maskY      = bg_info[bg_id].maskY;

  bg.y           = io.bg_y[bg_id];
  uint16 hscroll = bg.io.hoffset;
  uint16 vscroll = bg.io.voffset;

  const uint hires = io.bgMode == 5 || io.bgMode == 6;
  const uint width = 256 << hires;

  if(hires) {
    hscroll <<= 1;
    if(io.interlace) bg.y = (bg.y << 1) + field();
  }

  uint16 hval, vval;
  uint16 hoffset, voffset, offsetX, col;
  bool   mirrorX, mirrorY;

  const uint8*  tile_ptr;
  const uint16* mtable = mosaicTable[bg.io.mosaicEnabled ? (uint)io.mosaicSize : 0];
  const bool isOPTMode = io.bgMode == 2 || io.bgMode == 4 || io.bgMode == 6;
  const bool isDirectColorMode = screen.io.directColor && bg_id == Background::ID::BG1 && (io.bgMode == 3 || io.bgMode == 4);

  buildWindowTables(bg_id);
  const uint8* wt_above = windowCache[bg_id].above;
  const uint8* wt_below = windowCache[bg_id].below;

  uint16 prev_x = 0xffff, prev_y = 0xffff, prev_optx = 0xffff;
  for(uint16 x : range(width)) {
    hoffset = mtable[x] + hscroll;
    voffset = bg.y + vscroll;

    if(isOPTMode) {
      offsetX = (x + (hscroll & 7));

      //tile 0 is unaffected by offset-per-tile mode...
      if(offsetX >= 8) {

        //cache tile data in hval, vval if possible
        if((offsetX >> 3) != (prev_optx >> 3)) {
          prev_optx = offsetX;

          hval = bg3.getTile((offsetX - 8) + (bg3.io.hoffset & ~7), bg3.io.voffset);
          if(io.bgMode != 4) {
            vval = bg3.getTile((offsetX - 8) + (bg3.io.hoffset & ~7), bg3.io.voffset + 8);
          }
        }

        if(io.bgMode == 4) {
          if(hval & validMask) {
            if(!(hval & 0x8000)) {
              hoffset = offsetX + (hval & ~7);
            } else {
              voffset = bg.y + hval;
            }
          }
        } else {
          if(hval & validMask) {
            hoffset = offsetX + (hval & ~7);
          }
          if(vval & validMask) {
            voffset = bg.y + vval;
          }
        }
      }
    }

    hoffset &= maskX;
    voffset &= maskY;

    if(hoffset >> 3 != prev_x || voffset >> 3 != prev_y) {
      prev_x = hoffset >> 3;
      prev_y = voffset >> 3;

      bg.tile = bg.getTile((uint)hoffset, (uint)voffset);  //format = vhopppcc cccccccc
      mirrorY = bg.tile & 0x8000;
      mirrorX = bg.tile & 0x4000;
      bg.priority = bg.io.priority[(bg.tile & 0x2000) >> 13];
      bg.paletteNumber = (bg.tile >> 10) & 7;
      bg.paletteIndex = paletteOffset + (bg.paletteNumber << paletteSize);

      if(tileWidth  == 4) {  //16x16 horizontal tile mirroring
        if((bool)(hoffset & 8) != mirrorX) bg.tile++;
      }

      if(tileHeight == 4) {  //16x16 vertical tile mirroring
        if((bool)(voffset & 8) != mirrorY) bg.tile += 16;
      }

      bg.tile &= 0x03ff;
      bg.tile += tiledataIndex;
      bg.tile &= tileMask;

      if(bg_tilestate[bg.tile] == 1) {
        renderBGTile(bg.io.mode, bg.tile);
      }

      if(mirrorY) voffset ^= 7;  //invert y tile pos
      tile_ptr = bg_tiledata + (bg.tile * 64) + ((voffset & 7) * 8);
    }

    if(mirrorX) hoffset ^= 7;  //invert x tile pos
    col = *(tile_ptr + (hoffset & 7));
    if(col) {
      if(isDirectColorMode) {
        col = screen.directColor(col, bg.tile);
      } else {
        col = screen.cgram[col + bg.paletteIndex];
      }

      #define setpixel_above(x) \
        if(pixelCache[x].abovePriority < bg.priority) { \
          pixelCache[x].abovePriority = bg.priority; \
          pixelCache[x].aboveLayer = bg_id; \
          pixelCache[x].aboveColor = col; \
          pixelCache[x].aboveColorExemption = false; \
        }

      #define setpixel_below(x) \
        if(pixelCache[x].belowPriority < bg.priority) { \
          pixelCache[x].belowPriority = bg.priority; \
          pixelCache[x].belowLayer = bg_id; \
          pixelCache[x].belowColor = col; \
          pixelCache[x].belowColorExemption = false; \
        }
      if(!hires) {
        if(bg.io.aboveEnable && !wt_above[x]) { setpixel_above(x); }
        if(bg.io.belowEnable && !wt_below[x]) { setpixel_below(x); }
      } else {
        int hx = x >> 1;
        if(x & 1) {
          if(bg.io.aboveEnable && !wt_above[hx]) { setpixel_above(hx); }
        } else {
          if(bg.io.belowEnable && !wt_below[hx]) { setpixel_below(hx); }
        }
      }
      #undef setpixel_above
      #undef setpixel_below
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

template<uint bg_id>
auto PPU::bg_renderLineMode7() -> void {
  Background& bg = (
  bg_id == Background::ID::BG1 ? bg1 :
  bg_id == Background::ID::BG2 ? bg2 :
  bg_id == Background::ID::BG3 ? bg3 :
                                 bg4
  );
  if(bg.io.mode == Background::Mode::Inactive) return;
  if(bg.io.priority[0] + bg.io.priority[1] == 0) return;
  if(!bg.io.aboveEnable && !bg.io.belowEnable) return;

  int32 px, py;
  int32 tx, ty, tile, palette;

  int32 a = sclip<16>(cache.m7a);
  int32 b = sclip<16>(cache.m7b);
  int32 c = sclip<16>(cache.m7c);
  int32 d = sclip<16>(cache.m7d);

  int32 cx   = sclip<13>(cache.m7x);
  int32 cy   = sclip<13>(cache.m7y);
  int32 hofs = sclip<13>(cache.hoffsetMode7);
  int32 vofs = sclip<13>(cache.voffsetMode7);

  int  _pri, _x;

  buildWindowTables(bg_id);
  uint8* wt_above = windowCache[bg_id].above;
  uint8* wt_below = windowCache[bg_id].below;

  bg.y = !io.vflipMode7 ? line : 255 - line;

  uint16* mtable_x;
  uint16* mtable_y;
  if(bg_id == Background::ID::BG1) {
    mtable_x = (uint16*)mosaicTable[bg1.io.mosaicEnabled ? (uint)io.mosaicSize : 0];
    mtable_y = (uint16*)mosaicTable[bg1.io.mosaicEnabled ? (uint)io.mosaicSize : 0];
  } else {  //bg_id == Background::ID::BG2
    //Mode7 EXTBG BG2 uses BG1 mosaic enable to control vertical mosaic,
    //and BG2 mosaic enable to control horizontal mosaic...
    mtable_x = (uint16*)mosaicTable[bg2.io.mosaicEnabled ? (uint)io.mosaicSize : 0];
    mtable_y = (uint16*)mosaicTable[bg1.io.mosaicEnabled ? (uint)io.mosaicSize : 0];
  }

  //13-bit sign extend
  //--s---vvvvvvvvvv -> ssssssvvvvvvvvvv
  #define CLIP(x) ( (x) & 0x2000 ? (x) | ~0x03ff : (x) & 0x03ff )

  int32 psx = ((a * CLIP(hofs - cx)) & ~63) + ((b * CLIP(vofs - cy)) & ~63) + ((b * mtable_y[bg.y]) & ~63) + (cx << 8);
  int32 psy = ((c * CLIP(hofs - cx)) & ~63) + ((d * CLIP(vofs - cy)) & ~63) + ((d * mtable_y[bg.y]) & ~63) + (cy << 8);

  #undef CLIP

  for(int32 x = 0; x < 256; x++) {
    px = psx + (a * mtable_x[x]);
    py = psy + (c * mtable_x[x]);

    //mask floating-point bits (low 8 bits)
    px >>= 8;
    py >>= 8;

    switch(io.repeatMode7) {
    //screen repetition outside of screen area
    case 0:
    case 1:
      px &= 1023;
      py &= 1023;
      tile = vram[(py >> 3) * 128 + (px >> 3)].byte(0);
      palette = vram[(tile << 6) + ((py & 7) << 3) + (px & 7)].byte(1);
      break;

    //palette color 0 outside of screen area
    case 2: 
      if((px | py) & ~1023) {
        palette = 0;
      } else {
        px &= 1023;
        py &= 1023;
        tile = vram[(py >> 3) * 128 + (px >> 3)].byte(0);
        palette = vram[(tile << 6) + ((py & 7) << 3) + (px & 7)].byte(1);
      }
      break;

    //character 0 repetition outside of screen area
    case 3:
      if((px | py) & ~1023) {
        tile = 0;
      } else {
        px &= 1023;
        py &= 1023;
        tile = vram[(py >> 3) * 128 + (px >> 3)].byte(0);
      }
      palette = vram[(tile << 6) + ((py & 7) << 3) + (px & 7)].byte(1);
      break;

    }

    if(bg_id == Background::ID::BG1) {
      _pri = bg.io.priority[0];
    } else {
      _pri = bg.io.priority[(palette >> 7) ? 1 : 0];
      palette &= 0x7f;
    }

    if(!palette) continue;

    _x = !io.hflipMode7 ? (uint)x : 255 - x;

    uint32 col;
    if(screen.io.directColor && bg_id == Background::ID::BG1) {
      //direct color mode does not apply to bg2, as it is only 128 colors...
      col = screen.directColor(palette, 0);
    } else {
      col = screen.cgram[palette];
    }

    if(bg.io.aboveEnable && !wt_above[_x]) {
      if(pixelCache[_x].abovePriority < _pri) {
        pixelCache[_x].abovePriority = _pri;
        pixelCache[_x].aboveLayer = bg_id;
        pixelCache[_x].aboveColor = col;
        pixelCache[_x].aboveColorExemption = false;
      }
    }
    if(bg.io.belowEnable && !wt_below[_x]) {
      if(pixelCache[_x].belowPriority < _pri) {
        pixelCache[_x].belowPriority = _pri;
        pixelCache[_x].belowLayer = bg_id;
        pixelCache[_x].belowColor = col;
        pixelCache[_x].belowColorExemption = false;
      }
    }
  }
}
