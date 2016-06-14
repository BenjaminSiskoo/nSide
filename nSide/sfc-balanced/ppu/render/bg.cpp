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
auto PPU::renderBGLine(uint mode) -> void {
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
  const uint8  bgpal_index   = mode == 0 ? bg_id << 5 : 0;

  const uint8  pal_size  = 2 << bg.r.mode;       //<<2 (*4), <<4 (*16), <<8 (*256)
  const uint16 tile_mask = 0x0fff >> bg.r.mode;  //0x0fff, 0x07ff, 0x03ff
  //4 + bg.r.mode = >>(4-6) -- / {16, 32, 64 } bytes/tile
  //index is a tile number count to add to base tile number
  const uint tiledata_index = bg.r.tiledataAddress >> (4 + bg.r.mode);

  const uint8 *bg_td       = bg_tiledata[bg.r.mode];
  const uint8 *bg_td_state = bg_tiledataState[bg.r.mode];

  const uint8  tile_width  = bg_info[bg_id].tw;
  const uint8  tile_height = bg_info[bg_id].th;
  const uint16 mask_x      = bg_info[bg_id].mx;  //screen width  mask
  const uint16 mask_y      = bg_info[bg_id].my;  //screen height mask

  uint16 y       = r.bg_y[bg_id];
  uint16 hscroll = bg.r.hoffset;
  uint16 vscroll = bg.r.voffset;

  const uint hires = mode == 5 || mode == 6;
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
  const bool    is_opt_mode = mode == 2 || mode == 4 || mode == 6;
  const bool    isDirectColorMode = screen.r.directColor && bg_id == Background::ID::BG1 && (mode == 3 || mode == 4);

  buildWindowTables(bg_id);
  const uint8* wt_above = windowCache[bg_id].above;
  const uint8* wt_below = windowCache[bg_id].below;

  uint16 prev_x = 0xffff, prev_y = 0xffff, prev_optx = 0xffff;
  for(uint16 x : range(width)) {
    hoffset = mtable[x] + hscroll;
    voffset = y + vscroll;

    if(is_opt_mode) {
      opt_x = x + (hscroll & 7);

      //tile 0 is unaffected by offset-per-tile mode...
      if(opt_x >= 8) {
        //cache tile data in hval, vval if possible
        if((opt_x >> 3) != (prev_optx >> 3)) {
          prev_optx = opt_x;

          hval = bg3.getTile((opt_x - 8) + (bg3.r.hoffset & ~7), bg3.r.voffset);
          if(mode != 4) {
            vval = bg3.getTile((opt_x - 8) + (bg3.r.hoffset & ~7), bg3.r.voffset + 8);
          }
        }

        if(mode == 4) {
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
      tile_pri  = tile_num & 0x2000 ? bg.r.priority[1] : bg.r.priority[0];
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

      if(bg_td_state[tile_num] == 1) {
        render_bg_tile(bg.r.mode, tile_num);
      }

      if(mirror_y) voffset ^= 7;  //invert y tile pos
      tile_ptr = bg_td + (tile_num * 64) + ((voffset & 7) * 8);
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
        if(pixelCache[x].pri_above < tile_pri) { \
          pixelCache[x].pri_above = tile_pri; \
          pixelCache[x].bg_above  = bg_id; \
          pixelCache[x].src_above = col; \
          pixelCache[x].ce_above  = false; \
        }

      #define setpixel_below(x) \
        if(pixelCache[x].pri_below < tile_pri) { \
          pixelCache[x].pri_below = tile_pri; \
          pixelCache[x].bg_below  = bg_id; \
          pixelCache[x].src_below = col; \
          pixelCache[x].ce_below  = false; \
        }
      if(!hires) {
        if(bg.r.aboveEnable && !wt_above[x]) { setpixel_above(x); }
        if(bg.r.belowEnable  && !wt_below[x])  { setpixel_below(x);  }
      } else {
        int hx = x >> 1;
        if(x & 1) {
          if(bg.r.aboveEnable && !wt_above[hx]) { setpixel_above(hx); }
        } else {
          if(bg.r.belowEnable  && !wt_below[hx])  { setpixel_below(hx);  }
        }
      }
      #undef setpixel_above
      #undef setpixel_below
    }
  }
}
