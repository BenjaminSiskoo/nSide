auto PPU::updateSpriteList(uint addr, uint8 data) -> void {
  if(addr < 0x0200) {
    uint i = addr >> 2;
    switch(addr & 3) {
    case 0: spriteList[i].x = (spriteList[i].x & 0x0100) | data; break;
    case 1: spriteList[i].y = (data + 1) & 0xff; break;
    case 2: spriteList[i].character = data; break;
    case 3: spriteList[i].vflip = data & 0x80;
            spriteList[i].hflip = data & 0x40;
            spriteList[i].priority = (data >> 4) & 3;
            spriteList[i].palette = (data >> 1) & 7;
            spriteList[i].nameSelect = data & 0x01;
    }
  } else {
    uint i = (addr & 0x1f) << 2;
    spriteList[i + 0].x = ((data & 0x01) << 8) | (spriteList[i + 0].x & 0xff);
    spriteList[i + 0].size = data & 0x02;
    spriteList[i + 1].x = ((data & 0x04) << 6) | (spriteList[i + 1].x & 0xff);
    spriteList[i + 1].size = data & 0x08;
    spriteList[i + 2].x = ((data & 0x10) << 4) | (spriteList[i + 2].x & 0xff);
    spriteList[i + 2].size = data & 0x20;
    spriteList[i + 3].x = ((data & 0x40) << 2) | (spriteList[i + 3].x & 0xff);
    spriteList[i + 3].size = data & 0x80;
  }
}

auto PPU::build_spriteList() -> void {
  if(spriteListValid) return;
  spriteListValid = true;

  for(uint i : range(128)) {
    const bool size = spriteList[i].size;

    switch(cache.oam_baseSize) {
    case 0: spriteList[i].width  = (!size) ?  8 : 16;
            spriteList[i].height = (!size) ?  8 : 16;
            break;
    case 1: spriteList[i].width  = (!size) ?  8 : 32;
            spriteList[i].height = (!size) ?  8 : 32;
            break;
    case 2: spriteList[i].width  = (!size) ?  8 : 64;
            spriteList[i].height = (!size) ?  8 : 64;
            break;
    case 3: spriteList[i].width  = (!size) ? 16 : 32;
            spriteList[i].height = (!size) ? 16 : 32;
            break;
    case 4: spriteList[i].width  = (!size) ? 16 : 64;
            spriteList[i].height = (!size) ? 16 : 64;
            break;
    case 5: spriteList[i].width  = (!size) ? 32 : 64;
            spriteList[i].height = (!size) ? 32 : 64;
            break;
    case 6: spriteList[i].width  = (!size) ? 16 : 32;
            spriteList[i].height = (!size) ? 32 : 64;
            if(oam.r.interlace && !size) spriteList[i].height = 16;
            //32x64 height is not affected by oam.r.interlace setting
            break;
    case 7: spriteList[i].width  = (!size) ? 16 : 32;
            spriteList[i].height = (!size) ? 32 : 32;
            if(oam.r.interlace && !size) spriteList[i].height = 16;
            break;
    }
  }
}

auto PPU::is_sprite_on_scanline() -> bool {
  //if sprite is entirely offscreen and doesn't wrap around to the left side of the screen,
  //then it is not counted. this *should* be 256, and not 255, even though dot 256 is offscreen.
  SpriteItem* spr = &spriteList[activeSprite];
  if(spr->x > 256 && (spr->x + spr->width - 1) < 512) return false;

  int spr_height = !oam.r.interlace ? (uint)spr->height : spr->height >> 1;
  if(line >= spr->y && line < (spr->y + spr_height)) return true;
  if((spr->y + spr_height) >= 256 && line < ((spr->y + spr_height) & 255)) return true;
  return false;
}

auto PPU::load_oam_tiles() -> void {
  SpriteItem* spr = &spriteList[activeSprite];
  uint16 tile_width = spr->width >> 3;
  int x = spr->x;
  int y = (line - spr->y) & 0xff;
  if(oam.r.interlace) {
    y <<= 1;
  }

  if(spr->vflip) {
    if(spr->width == spr->height) {
      y = (spr->height - 1) - y;
    } else {
      y = y < spr->width ? (spr->width - 1) - y : spr->width + ((spr->width - 1) - (y - spr->width));
    }
  }

  if(oam.r.interlace) {
    y = !spr->vflip ? y + field() : y - field();
  }

  x &= 511;
  y &= 255;

  uint16 tdaddr = cache.oam_tiledataAddress;
  uint16 chrx   = (spr->character     ) & 15;
  uint16 chry   = (spr->character >> 4) & 15;
  if(spr->nameSelect) {
    tdaddr += (256 * 32) + (cache.oam_nameSelect << 13);
  }
  chry  += (y >> 3);
  chry  &= 15;
  chry <<= 4;

  for(uint tx : range(tile_width)) {
    uint sx = (x + (tx << 3)) & 511;
    //ignore sprites that are offscreen, x==256 is a special case that loads all tiles in OBJ
    if(x != 256 && sx >= 256 && (sx + 7) < 512) continue;

    if(oam.t.tileCount++ >= 34) break;
    uint n = oam.t.tileCount - 1;
    oam_tileList[n].x     = sx;
    oam_tileList[n].y     = y;
    oam_tileList[n].pri   = spr->priority;
    oam_tileList[n].pal   = 128 + (spr->palette << 4);
    oam_tileList[n].hflip = spr->hflip;

    uint mx  = !spr->hflip ? tx : (tile_width - 1) - tx;
    uint pos = tdaddr + ((chry + ((chrx + mx) & 15)) << 5);
    oam_tileList[n].tile = (pos >> 5) & 0x07ff;
  }
}

auto PPU::render_oam_tile(int tile_num) -> void {
  oam_tileitem* t     = &oam_tileList[tile_num];
  uint8* oam_td       = (uint8*)bg_tiledata[Background::Mode::BPP4];
  uint8* oam_td_state = (uint8*)bg_tiledataState[Background::Mode::BPP4];

  if(oam_td_state[t->tile] == 1) {
    render_bg_tile(Background::Mode::BPP4, t->tile);
  }

  uint sx = t->x;
  uint8* tile_ptr = (uint8*)oam_td + (t->tile << 6) + ((t->y & 7) << 3);
  for(uint x : range(8)) {
    sx &= 511;
    if(sx < 256) {
      uint col = *(tile_ptr + (!t->hflip ? x : 7 - x));
      if(col) {
        col += t->pal;
        oam_linePalette[sx] = col;
        oam_linePriority[sx] = t->pri;
      }
    }
    sx++;
  }
}

auto PPU::renderLine_oam_rto() -> void {
  build_spriteList();

  oam.t.itemCount = 0;
  oam.t.tileCount = 0;

  memset(oam_linePriority, OAM_PRI_NONE, 256);
  memset(oam_itemList, 0xff, 32);
  for(int s : range(34)) oam_tileList[s].tile = 0xffff;

  for(int s : range(128)) {
    activeSprite = (s + oam.r.firstSprite) & 127;
    if(!is_sprite_on_scanline()) continue;
    if(oam.t.itemCount++ >= 32) break;
    oam_itemList[oam.t.itemCount - 1] = (s + oam.r.firstSprite) & 127;
  }

  if(oam.t.itemCount > 0 && oam_itemList[oam.t.itemCount - 1] != 0xff) {
    latch.oamAddress = 0x0200 + (oam_itemList[oam.t.itemCount - 1] >> 2);
  }

  for(int s = 31; s >= 0; s--) {
    if(oam_itemList[s] == 0xff) continue;
    activeSprite = oam_itemList[s];
    load_oam_tiles();
  }

  oam.r.timeOver  |= (oam.t.tileCount > 34);
  oam.r.rangeOver |= (oam.t.itemCount > 32);
}

auto PPU::renderLine_oam(uint8 pri0_pos, uint8 pri1_pos, uint8 pri2_pos, uint8 pri3_pos) -> void {
  if(pri0_pos + pri1_pos + pri2_pos + pri3_pos == 0) return;

  if(!oam.r.aboveEnable && !oam.r.belowEnable) return;

  for(uint s : range(34)) {
    if(oam_tileList[s].tile == 0xffff) continue;
    render_oam_tile(s);
  }

  buildWindowTables(OAM::ID::OAM);
  uint8* wt_above = windowCache[OAM::ID::OAM].above;
  uint8* wt_below = windowCache[OAM::ID::OAM].below;

  uint pri_tbl[4] = { pri0_pos, pri1_pos, pri2_pos, pri3_pos };
  #define setpixel_above(x) \
    if(pixelCache[x].pri_above < pri) { \
      pixelCache[x].pri_above = pri; \
      pixelCache[x].bg_above  = OAM::ID::OAM; \
      pixelCache[x].src_above = getPalette(oam_linePalette[x]); \
      pixelCache[x].ce_above  = (oam_linePalette[x] < 192); \
    }
  #define setpixel_below(x) \
    if(pixelCache[x].pri_below < pri) { \
      pixelCache[x].pri_below = pri; \
      pixelCache[x].bg_below  = OAM::ID::OAM; \
      pixelCache[x].src_below = getPalette(oam_linePalette[x]); \
      pixelCache[x].ce_below  = (oam_linePalette[x] < 192); \
    }
  for(int x : range(256)) {
    if(oam_linePriority[x] == OAM_PRI_NONE) continue;

    uint pri = pri_tbl[oam_linePriority[x]];
    if(oam.r.aboveEnable && !wt_above[x]) { setpixel_above(x); }
    if(oam.r.belowEnable && !wt_below[x]) { setpixel_below(x); }
  }
  #undef setpixel_above
  #undef setpixel_below
}
