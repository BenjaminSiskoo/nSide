//called once at the start of every rendered scanline
auto PPU::updateBGInfo() -> void {
  const uint hires = r.bgMode == 5 || r.bgMode == 6;
  const uint width = !hires ? 256 : 512;

  auto bg = &bg1;
  for(uint bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) {
    switch(bg_id) {
    case Background::ID::BG1: bg = &bg1; break;
    case Background::ID::BG2: bg = &bg2; break;
    case Background::ID::BG3: bg = &bg3; break;
    case Background::ID::BG4: bg = &bg4; break;
    }

    bg_info[bg_id].th = bg->r.tileSize ? 4 : 3;
    bg_info[bg_id].tw = hires ? 4 : (uint)bg_info[bg_id].th;

    bg_info[bg_id].mx = bg_info[bg_id].th == 4 ? width << 1 : width;
    bg_info[bg_id].my = bg_info[bg_id].mx;
    if(bg->r.screenSize & 0x01) bg_info[bg_id].mx <<= 1;
    if(bg->r.screenSize & 0x02) bg_info[bg_id].my <<= 1;
    bg_info[bg_id].mx--;
    bg_info[bg_id].my--;

    bg_info[bg_id].scy = bg->r.screenSize & 0x02 ? 32 << 5 : 0;
    bg_info[bg_id].scx = bg->r.screenSize & 0x01 ? 32 << 5 : 0;
    if(bg->r.screenSize == Background::ScreenSize::Size64x64) bg_info[bg_id].scy <<= 1;
  }
}

template<uint bg_id>
auto PPU::bg_renderLine() -> void {
  if(r.bgMode == 7) return bg_renderLineMode7<bg_id>();
  Background& bg = (
  bg_id == Background::ID::BG1 ? bg1 :
  bg_id == Background::ID::BG2 ? bg2 :
  bg_id == Background::ID::BG3 ? bg3 :
                                 bg4
  );
  if(bg.r.mode == Background::Mode::Inactive) return;
  if(bg.r.priority[0] + bg.r.priority[1] == 0) return;
  if(!bg.r.aboveEnable && !bg.r.belowEnable) return;

  const uint16 opt_valid_bit = bg_id == Background::ID::BG1 ? 0x2000 : bg_id == Background::ID::BG2 ? 0x4000 : 0x0000;
  const uint8  bgpal_index   = r.bgMode == 0 ? bg_id << 5 : 0;

  const uint8  pal_size  = 2 << bg.r.mode;       //<<2 (*4), <<4 (*16), <<8 (*256)
  const uint16 tile_mask = 0x0fff >> bg.r.mode;  //0x0fff, 0x07ff, 0x03ff
  //4 + bg.r.mode = >>(4-6) -- / {16, 32, 64 } bytes/tile
  //index is a tile number count to add to base tile number
  const uint tiledata_index = bg.r.tiledataAddress >> (3 + bg.r.mode);

  const uint8* bg_tiledata  = tiledataCache.tiledata[bg.r.mode];
  const uint8* bg_tilestate = tiledataCache.tiledataState[bg.r.mode];

  const uint8  tile_width  = bg_info[bg_id].tw;
  const uint8  tile_height = bg_info[bg_id].th;
  const uint16 mask_x      = bg_info[bg_id].mx;  //screen width  mask
  const uint16 mask_y      = bg_info[bg_id].my;  //screen height mask

  uint16 y       = r.bg_y[bg_id];
  uint16 hscroll = bg.r.hoffset;
  uint16 vscroll = bg.r.voffset;

  const uint hires = r.bgMode == 5 || r.bgMode == 6;
  const uint width = !hires ? 256 : 512;

  if(hires) {
    hscroll <<= 1;
    if(r.interlace) y = (y << 1) + field();
  }

  uint16 hval, vval;
  uint16 tile_pri, tile_num;
  uint8  pal_index, pal_num;
  uint16 hoffset, voffset, opt_x, col;
  bool   mirror_x, mirror_y;

  const uint8*  tile_ptr;
  const uint16* mtable = mosaicTable[bg.r.mosaicEnabled ? (uint)r.mosaicSize : 0];
  const bool isOPTMode = r.bgMode == 2 || r.bgMode == 4 || r.bgMode == 6;
  const bool isDirectColorMode = screen.r.directColor && bg_id == Background::ID::BG1 && (r.bgMode == 3 || r.bgMode == 4);

  buildWindowTables(bg_id);
  const uint8* wt_above = windowCache[bg_id].above;
  const uint8* wt_below = windowCache[bg_id].below;

  uint16 prev_x = 0xffff, prev_y = 0xffff, prev_optx = 0xffff;
  for(uint16 x : range(width)) {
    hoffset = mtable[x] + hscroll;
    voffset = y + vscroll;

    if(isOPTMode) {
      opt_x = x + (hscroll & 7);

      //tile 0 is unaffected by offset-per-tile mode...
      if(opt_x >= 8) {
        //cache tile data in hval, vval if possible
        if((opt_x >> 3) != (prev_optx >> 3)) {
          prev_optx = opt_x;

          hval = bg3.getTile((opt_x - 8) + (bg3.r.hoffset & ~7), bg3.r.voffset);
          if(r.bgMode != 4) {
            vval = bg3.getTile((opt_x - 8) + (bg3.r.hoffset & ~7), bg3.r.voffset + 8);
          }
        }

        if(r.bgMode == 4) {
          if(hval & opt_valid_bit) {
            if(!(hval & 0x8000)) {
              hoffset = opt_x + (hval & ~7);
            } else {
              voffset = y + hval;
            }
          }
        } else {
          if(hval & opt_valid_bit) {
            hoffset = opt_x + (hval & ~7);
          }
          if(vval & opt_valid_bit) {
            voffset = y + vval;
          }
        }
      }
    }

    hoffset &= mask_x;
    voffset &= mask_y;

    if(hoffset >> 3 != prev_x || voffset >> 3 != prev_y) {
      prev_x = hoffset >> 3;
      prev_y = voffset >> 3;

      tile_num  = bg.getTile((uint)hoffset, (uint)voffset);  //format = vhopppcc cccccccc
      mirror_y  = tile_num & 0x8000;
      mirror_x  = tile_num & 0x4000;
      tile_pri  = bg.r.priority[(tile_num & 0x2000) >> 13];
      pal_num   = (tile_num >> 10) & 7;
      pal_index = bgpal_index + (pal_num << pal_size);

      if(tile_width  == 4) {  //16x16 horizontal tile mirroring
        if((bool)(hoffset & 8) != mirror_x) tile_num++;
      }

      if(tile_height == 4) {  //16x16 vertical tile mirroring
        if((bool)(voffset & 8) != mirror_y) tile_num += 16;
      }

      tile_num &= 0x03ff;
      tile_num += tiledata_index;
      tile_num &= tile_mask;

      if(bg_tilestate[tile_num] == 1) {
        renderBGTile(bg.r.mode, tile_num);
      }

      if(mirror_y) voffset ^= 7;  //invert y tile pos
      tile_ptr = bg_tiledata + (tile_num * 64) + ((voffset & 7) * 8);
    }

    if(mirror_x) hoffset ^= 7;  //invert x tile pos
    col = *(tile_ptr + (hoffset & 7));
    if(col) {
      if(isDirectColorMode) {
        col = getDirectColor(pal_num, col);
      } else {
        col = getPalette(col + pal_index);
      }

      #define setpixel_above(x) \
        if(pixelCache[x].abovePriority < tile_pri) { \
          pixelCache[x].abovePriority = tile_pri; \
          pixelCache[x].aboveLayer = bg_id; \
          pixelCache[x].aboveColor = col; \
          pixelCache[x].aboveColorExemption = false; \
        }

      #define setpixel_below(x) \
        if(pixelCache[x].belowPriority < tile_pri) { \
          pixelCache[x].belowPriority = tile_pri; \
          pixelCache[x].belowLayer = bg_id; \
          pixelCache[x].belowColor = col; \
          pixelCache[x].belowColorExemption = false; \
        }
      if(!hires) {
        if(bg.r.aboveEnable && !wt_above[x]) { setpixel_above(x); }
        if(bg.r.belowEnable && !wt_below[x]) { setpixel_below(x); }
      } else {
        int hx = x >> 1;
        if(x & 1) {
          if(bg.r.aboveEnable && !wt_above[hx]) { setpixel_above(hx); }
        } else {
          if(bg.r.belowEnable && !wt_below[hx]) { setpixel_below(hx); }
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
  if(bg.r.mode == Background::Mode::Inactive) return;
  if(bg.r.priority[0] + bg.r.priority[1] == 0) return;
  if(!bg.r.aboveEnable && !bg.r.belowEnable) return;

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

  int32 y = !r.vflipMode7 ? line : 255 - line;

  uint16* mtable_x;
  uint16* mtable_y;
  if(bg_id == Background::ID::BG1) {
    mtable_x = (uint16*)mosaicTable[bg1.r.mosaicEnabled ? (uint)r.mosaicSize : 0];
    mtable_y = (uint16*)mosaicTable[bg1.r.mosaicEnabled ? (uint)r.mosaicSize : 0];
  } else {  //bg_id == Background::ID::BG2
    //Mode7 EXTBG BG2 uses BG1 mosaic enable to control vertical mosaic,
    //and BG2 mosaic enable to control horizontal mosaic...
    mtable_x = (uint16*)mosaicTable[bg2.r.mosaicEnabled ? (uint)r.mosaicSize : 0];
    mtable_y = (uint16*)mosaicTable[bg1.r.mosaicEnabled ? (uint)r.mosaicSize : 0];
  }

  //13-bit sign extend
  //--s---vvvvvvvvvv -> ssssssvvvvvvvvvv
  #define CLIP(x) ( (x) & 0x2000 ? (x) | ~0x03ff : (x) & 0x03ff )

  int32 psx = ((a * CLIP(hofs - cx)) & ~63) + ((b * CLIP(vofs - cy)) & ~63) + ((b * mtable_y[y]) & ~63) + (cx << 8);
  int32 psy = ((c * CLIP(hofs - cx)) & ~63) + ((d * CLIP(vofs - cy)) & ~63) + ((d * mtable_y[y]) & ~63) + (cy << 8);

  #undef CLIP

  for(int32 x = 0; x < 256; x++) {
    px = psx + (a * mtable_x[x]);
    py = psy + (c * mtable_x[x]);

    //mask floating-point bits (low 8 bits)
    px >>= 8;
    py >>= 8;

    switch(r.repeatMode7) {
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
      _pri = bg.r.priority[0];
    } else {
      _pri = bg.r.priority[(palette >> 7) ? 1 : 0];
      palette &= 0x7f;
    }

    if(!palette) continue;

    _x = !r.hflipMode7 ? (uint)x : 255 - x;

    uint32 col;
    if(screen.r.directColor && bg_id == Background::ID::BG1) {
      //direct color mode does not apply to bg2, as it is only 128 colors...
      col = getDirectColor(0, palette);
    } else {
      col = getPalette(palette);
    }

    if(bg.r.aboveEnable && !wt_above[_x]) {
      if(pixelCache[_x].abovePriority < _pri) {
        pixelCache[_x].abovePriority = _pri;
        pixelCache[_x].aboveLayer = bg_id;
        pixelCache[_x].aboveColor = col;
        pixelCache[_x].aboveColorExemption = false;
      }
    }
    if(bg.r.belowEnable && !wt_below[_x]) {
      if(pixelCache[_x].belowPriority < _pri) {
        pixelCache[_x].belowPriority = _pri;
        pixelCache[_x].belowLayer = bg_id;
        pixelCache[_x].belowColor = col;
        pixelCache[_x].belowColorExemption = false;
      }
    }
  }
}
