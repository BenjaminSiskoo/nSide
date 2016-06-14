//higan mode7 renderer
//
//base algorithm written by anomie
//higan implementation written by byuu
//
//supports mode 7 + extbg + rotate + zoom + direct color + scrolling + m7sel + windowing + mosaic
//interlace and pseudo-hires support are automatic via main rendering routine

template<uint bg_id>
auto PPU::renderMode7Line() -> void {
  Background& bg = (
  bg_id == Background::ID::BG1 ? bg1 :
  bg_id == Background::ID::BG2 ? bg2 :
  bg_id == Background::ID::BG3 ? bg3 :
                                 bg4
  );
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
    case 0:    //screen repetition outside of screen area
    case 1: {  //same as case 0
      px &= 1023;
      py &= 1023;
      tx = ((px >> 3) & 127);
      ty = ((py >> 3) & 127);
      tile    = memory.vram[(ty * 128 + tx) << 1];
      palette = memory.vram[(((tile << 6) + ((py & 7) << 3) + (px & 7)) << 1) + 1];
    } break;
    case 2: {  //palette color 0 outside of screen area
      if((px | py) & ~1023) {
        palette = 0;
      } else {
        px &= 1023;
        py &= 1023;
        tx = ((px >> 3) & 127);
        ty = ((py >> 3) & 127);
        tile    = memory.vram[(ty * 128 + tx) << 1];
        palette = memory.vram[(((tile << 6) + ((py & 7) << 3) + (px & 7)) << 1) + 1];
      }
    } break;
    case 3: {  //character 0 repetition outside of screen area
      if((px | py) & ~1023) {
        tile = 0;
      } else {
        px &= 1023;
        py &= 1023;
        tx = ((px >> 3) & 127);
        ty = ((py >> 3) & 127);
        tile = memory.vram[(ty * 128 + tx) << 1];
      }
      palette = memory.vram[(((tile << 6) + ((py & 7) << 3) + (px & 7)) << 1) + 1];
    } break;
    }

    if(bg_id == Background::ID::BG1) {
      _pri = bg.r.priority[0];
    } else {
      _pri = (palette >> 7) ? bg.r.priority[1] : bg.r.priority[0];
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
      if(pixelCache[_x].pri_above < _pri) {
        pixelCache[_x].pri_above = _pri;
        pixelCache[_x].bg_above  = bg_id;
        pixelCache[_x].src_above = col;
        pixelCache[_x].ce_above  = false;
      }
    }
    if(bg.r.belowEnable && !wt_below[_x]) {
      if(pixelCache[_x].pri_below < _pri) {
        pixelCache[_x].pri_below = _pri;
        pixelCache[_x].bg_below  = bg_id;
        pixelCache[_x].src_below = col;
        pixelCache[_x].ce_below  = false;
      }
    }
  }
}
