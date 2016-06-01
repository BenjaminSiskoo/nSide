//called once at the start of every rendered scanline
auto PPU::update_bg_info() -> void {
  const uint hires = regs.bgmode == 5 || regs.bgmode == 6;
  const uint width = !hires ? 256 : 512;

  auto bg = &bg1;
  for(uint bg_id = Background::ID::BG1; bg_id <= Background::ID::BG4; bg_id++) {
    switch(bg_id) {
    case Background::ID::BG1: bg = &bg1; break;
    case Background::ID::BG2: bg = &bg2; break;
    case Background::ID::BG3: bg = &bg3; break;
    case Background::ID::BG4: bg = &bg4; break;
    }

    bg_info[bg_id].th = bg->regs.tile_size ? 4 : 3;
    bg_info[bg_id].tw = hires ? 4 : (uint)bg_info[bg_id].th;

    bg_info[bg_id].mx = bg_info[bg_id].th == 4 ? width << 1 : width;
    bg_info[bg_id].my = bg_info[bg_id].mx;
    if(bg->regs.screen_size & 0x01) bg_info[bg_id].mx <<= 1;
    if(bg->regs.screen_size & 0x02) bg_info[bg_id].my <<= 1;
    bg_info[bg_id].mx--;
    bg_info[bg_id].my--;

    bg_info[bg_id].scy = bg->regs.screen_size & 0x02 ? 32 << 5 : 0;
    bg_info[bg_id].scx = bg->regs.screen_size & 0x01 ? 32 << 5 : 0;
    if(bg->regs.screen_size == Background::ScreenSize::Size64x64) bg_info[bg_id].scy <<= 1;
  }
}

template<uint bg_id>
auto PPU::bg_get_tile(uint16 x, uint16 y) -> uint16 {
  x = (x & bg_info[bg_id].mx) >> bg_info[bg_id].tw;
  y = (y & bg_info[bg_id].my) >> bg_info[bg_id].th;

  uint16 pos = ((y & 0x1f) << 5) + (x & 0x1f);
  if(y & 0x20) pos += bg_info[bg_id].scy;
  if(x & 0x20) pos += bg_info[bg_id].scx;

  auto bg = &bg1;
  switch(bg_id) {
  case Background::ID::BG1: bg = &bg1; break;
  case Background::ID::BG2: bg = &bg2; break;
  case Background::ID::BG3: bg = &bg3; break;
  case Background::ID::BG4: bg = &bg4; break;
  }
  const uint16 addr = bg->regs.screen_addr + (pos << 1);
  return vram[addr] + (vram[addr + 1] << 8);
}

template<uint mode, uint bg_id, uint color_depth>
auto PPU::render_line_bg(uint8 pri0_pos, uint8 pri1_pos) -> void {
  if(pri0_pos + pri1_pos == 0) return;

  auto bg = &bg1;
  switch(bg_id) {
  case Background::ID::BG1: bg = &bg1; break;
  case Background::ID::BG2: bg = &bg2; break;
  case Background::ID::BG3: bg = &bg3; break;
  case Background::ID::BG4: bg = &bg4; break;
  }
  if(!bg->regs.main_enable && !bg->regs.sub_enable) return;

  const uint16 opt_valid_bit = bg_id == Background::ID::BG1 ? 0x2000 : bg_id == Background::ID::BG2 ? 0x4000 : 0x0000;
  const uint8  bgpal_index   = mode == 0 ? bg_id << 5 : 0;

  const uint8  pal_size  = 2 << color_depth;       //<<2 (*4), <<4 (*16), <<8 (*256)
  const uint16 tile_mask = 0x0fff >> color_depth;  //0x0fff, 0x07ff, 0x03ff
  //4 + color_depth = >>(4-6) -- / {16, 32, 64 } bytes/tile
  //index is a tile number count to add to base tile number
  const uint tiledata_index = bg->regs.tiledata_addr >> (4 + color_depth);

  const uint8 *bg_td       = bg_tiledata[color_depth];
  const uint8 *bg_td_state = bg_tiledata_state[color_depth];

  const uint8  tile_width  = bg_info[bg_id].tw;
  const uint8  tile_height = bg_info[bg_id].th;
  const uint16 mask_x      = bg_info[bg_id].mx;  //screen width  mask
  const uint16 mask_y      = bg_info[bg_id].my;  //screen height mask

  uint16 y       = regs.bg_y[bg_id];
  uint16 hscroll = bg->regs.hoffset;
  uint16 vscroll = bg->regs.voffset;

  const uint hires = mode == 5 || mode == 6;
  const uint width = !hires ? 256 : 512;

  if(hires) {
    hscroll <<= 1;
    if(regs.interlace) y = (y << 1) + field();
  }

  uint16 hval, vval;
  uint16 tile_pri, tile_num;
  uint8  pal_index, pal_num;
  uint16 hoffset, voffset, opt_x, col;
  bool   mirror_x, mirror_y;

  const uint8*  tile_ptr;
  const uint16* mtable = mosaic_table[bg->regs.mosaic_enabled ? (uint)regs.mosaic_size : 0];
  const bool    is_opt_mode = mode == 2 || mode == 4 || mode == 6;
  const bool    is_direct_color_mode = screen.regs.direct_color && bg_id == Background::ID::BG1 && (mode == 3 || mode == 4);

  build_window_tables(bg_id);
  const uint8* wt_main = windowCache[bg_id].main;
  const uint8* wt_sub  = windowCache[bg_id].sub;

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

          hval = bg_get_tile<Background::ID::BG3>((opt_x - 8) + (bg3.regs.hoffset & ~7), bg3.regs.voffset);
          if(mode != 4) {
            vval = bg_get_tile<Background::ID::BG3>((opt_x - 8) + (bg3.regs.hoffset & ~7), bg3.regs.voffset + 8);
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

      tile_num  = bg_get_tile<bg_id>(hoffset, voffset);  //format = vhopppcc cccccccc
      mirror_y  = tile_num & 0x8000;
      mirror_x  = tile_num & 0x4000;
      tile_pri  = tile_num & 0x2000 ? pri1_pos : pri0_pos;
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
        render_bg_tile<color_depth>(tile_num);
      }

      if(mirror_y) voffset ^= 7;  //invert y tile pos
      tile_ptr = bg_td + (tile_num * 64) + ((voffset & 7) * 8);
    }

    if(mirror_x) hoffset ^= 7;  //invert x tile pos
    col = *(tile_ptr + (hoffset & 7));
    if(col) {
      if(is_direct_color_mode) {
        col = get_direct_color(pal_num, col);
      } else {
        col = get_palette(col + pal_index);
      }

      #define setpixel_main(x) \
        if(pixelCache[x].pri_main < tile_pri) { \
          pixelCache[x].pri_main = tile_pri; \
          pixelCache[x].bg_main  = bg_id; \
          pixelCache[x].src_main = col; \
          pixelCache[x].ce_main  = false; \
        }

      #define setpixel_sub(x) \
        if(pixelCache[x].pri_sub < tile_pri) { \
          pixelCache[x].pri_sub = tile_pri; \
          pixelCache[x].bg_sub  = bg_id; \
          pixelCache[x].src_sub = col; \
          pixelCache[x].ce_sub  = false; \
        }
      if(!hires) {
        if(bg->regs.main_enable && !wt_main[x]) { setpixel_main(x); }
        if(bg->regs.sub_enable  && !wt_sub[x])  { setpixel_sub(x);  }
      } else {
        int hx = x >> 1;
        if(x & 1) {
          if(bg->regs.main_enable && !wt_main[hx]) { setpixel_main(hx); }
        } else {
          if(bg->regs.sub_enable  && !wt_sub[hx])  { setpixel_sub(hx);  }
        }
      }
      #undef setpixel_main
      #undef setpixel_sub
    }
  }
}
