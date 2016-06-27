auto PPU::updateSpriteList(uint addr, uint8 data) -> void {
  if(addr < 0x0200) {
    uint i = addr >> 2;
    switch(addr & 3) {
    case 0: obj.list[i].x = (obj.list[i].x & 0x0100) | data; break;
    case 1: obj.list[i].y = (data + 1) & 0xff; break;
    case 2: obj.list[i].character = data; break;
    case 3: obj.list[i].vflip = data & 0x80;
            obj.list[i].hflip = data & 0x40;
            obj.list[i].priority = (data >> 4) & 3;
            obj.list[i].palette = (data >> 1) & 7;
            obj.list[i].nameSelect = data & 0x01;
    }
  } else {
    uint i = (addr & 0x1f) << 2;
    obj.list[i + 0].x = ((data & 0x01) << 8) | (obj.list[i + 0].x & 0xff);
    obj.list[i + 0].size = data & 0x02;
    obj.list[i + 1].x = ((data & 0x04) << 6) | (obj.list[i + 1].x & 0xff);
    obj.list[i + 1].size = data & 0x08;
    obj.list[i + 2].x = ((data & 0x10) << 4) | (obj.list[i + 2].x & 0xff);
    obj.list[i + 2].size = data & 0x20;
    obj.list[i + 3].x = ((data & 0x40) << 2) | (obj.list[i + 3].x & 0xff);
    obj.list[i + 3].size = data & 0x80;
  }
}

auto PPU::buildSpriteList() -> void {
  if(spriteListValid) return;
  spriteListValid = true;

  for(uint i : range(128)) {
    const bool size = obj.list[i].size;

    switch(cache.obj_baseSize) {
    case 0: obj.list[i].width  = !size ?  8 : 16;
            obj.list[i].height = !size ?  8 : 16;
            break;
    case 1: obj.list[i].width  = !size ?  8 : 32;
            obj.list[i].height = !size ?  8 : 32;
            break;
    case 2: obj.list[i].width  = !size ?  8 : 64;
            obj.list[i].height = !size ?  8 : 64;
            break;
    case 3: obj.list[i].width  = !size ? 16 : 32;
            obj.list[i].height = !size ? 16 : 32;
            break;
    case 4: obj.list[i].width  = !size ? 16 : 64;
            obj.list[i].height = !size ? 16 : 64;
            break;
    case 5: obj.list[i].width  = !size ? 32 : 64;
            obj.list[i].height = !size ? 32 : 64;
            break;
    case 6: obj.list[i].width  = !size ? 16 : 32;
            obj.list[i].height = !size ? 32 : 64;
            if(obj.r.interlace && !size) obj.list[i].height = 16;
            //32x64 height is not affected by obj.r.interlace setting
            break;
    case 7: obj.list[i].width  = !size ? 16 : 32;
            obj.list[i].height = !size ? 32 : 32;
            if(obj.r.interlace && !size) obj.list[i].height = 16;
            break;
    }
  }
}

auto PPU::isSpriteOnScanline() -> bool {
  //if sprite is entirely offscreen and doesn't wrap around to the left side of the screen,
  //then it is not counted. this *should* be 256, and not 255, even though dot 256 is offscreen.
  Object::Sprite* spr = &obj.list[activeSprite];
  if(spr->x > 256 && (spr->x + spr->width - 1) < 512) return false;

  int spr_height = !obj.r.interlace ? (uint)spr->height : spr->height >> 1;
  if(line >= spr->y && line < (spr->y + spr_height)) return true;
  if((spr->y + spr_height) >= 256 && line < ((spr->y + spr_height) & 255)) return true;
  return false;
}

auto PPU::obj_loadTiles() -> void {
  Object::Sprite* spr = &obj.list[activeSprite];
  uint16 tile_width = spr->width >> 3;
  int x = spr->x;
  int y = (line - spr->y) & 0xff;
  if(obj.r.interlace) {
    y <<= 1;
  }

  if(spr->vflip) {
    if(spr->width == spr->height) {
      y = (spr->height - 1) - y;
    } else {
      y = y < spr->width ? (spr->width - 1) - y : spr->width + ((spr->width - 1) - (y - spr->width));
    }
  }

  if(obj.r.interlace) {
    y = !spr->vflip ? y + field() : y - field();
  }

  x &= 511;
  y &= 255;

  uint16 tdaddr = cache.obj_tiledataAddress;
  uint16 chrx   = (spr->character     ) & 15;
  uint16 chry   = (spr->character >> 4) & 15;
  if(spr->nameSelect) {
    tdaddr += (256 * 16) + (cache.obj_nameSelect << 12);
  }
  chry  += (y >> 3);
  chry  &= 15;
  chry <<= 4;

  for(uint tx : range(tile_width)) {
    uint sx = (x + (tx << 3)) & 511;
    //ignore sprites that are offscreen, x==256 is a special case that loads all tiles in OBJ
    if(x != 256 && sx >= 256 && (sx + 7) < 512) continue;

    if(obj.t.tileCount++ >= 34) break;
    uint n = obj.t.tileCount - 1;
    obj_tileList[n].x        = sx;
    obj_tileList[n].y        = y;
    obj_tileList[n].priority = spr->priority;
    obj_tileList[n].palette  = 128 + (spr->palette << 4);
    obj_tileList[n].hflip    = spr->hflip;

    uint mx  = !spr->hflip ? tx : (tile_width - 1) - tx;
    uint pos = tdaddr + ((chry + ((chrx + mx) & 15)) << 4);
    obj_tileList[n].tile = (pos >> 4) & (ppu.vram.mask >> 4);
  }
}

auto PPU::obj_renderTile(int n) -> void {
  uint8* obj_tileData      = (uint8*)tiledataCache.tiledata[Background::Mode::BPP4];
  uint8* obj_tileDataState = (uint8*)tiledataCache.tiledataState[Background::Mode::BPP4];

  if(obj_tileDataState[obj_tileList[n].tile] == 1) {
    renderBGTile(Background::Mode::BPP4, obj_tileList[n].tile);
  }

  uint sx = obj_tileList[n].x;
  uint8* tile_ptr = (uint8*)obj_tileData + (obj_tileList[n].tile << 6) + ((obj_tileList[n].y & 7) << 3);
  for(uint x : range(8)) {
    sx &= 511;
    if(sx < 256) {
      uint col = *(tile_ptr + (!obj_tileList[n].hflip ? x : 7 - x));
      if(col) {
        col += obj_tileList[n].palette;
        obj_linePalette[sx] = col;
        obj_linePriority[sx] = obj_tileList[n].priority;
      }
    }
    sx++;
  }
}

auto PPU::obj_renderLine_rto() -> void {
  buildSpriteList();

  obj.t.itemCount = 0;
  obj.t.tileCount = 0;

  memset(obj_linePriority, OBJ_PRI_NONE, 256);
  memset(obj_itemList, 0xff, 32);
  for(int s : range(34)) obj_tileList[s].tile = 0xffff;

  for(int s : range(128)) {
    activeSprite = (s + obj.r.firstSprite) & 127;
    if(!isSpriteOnScanline()) continue;
    if(obj.t.itemCount++ >= 32) break;
    obj_itemList[obj.t.itemCount - 1] = (s + obj.r.firstSprite) & 127;
  }

  if(obj.t.itemCount > 0 && obj_itemList[obj.t.itemCount - 1] != 0xff) {
    latch.oamAddress = 0x0200 + (obj_itemList[obj.t.itemCount - 1] >> 2);
  }

  for(int s = 31; s >= 0; s--) {
    if(obj_itemList[s] == 0xff) continue;
    activeSprite = obj_itemList[s];
    obj_loadTiles();
  }

  obj.r.timeOver  |= (obj.t.tileCount > 34);
  obj.r.rangeOver |= (obj.t.itemCount > 32);
}

auto PPU::obj_renderLine() -> void {
  if(obj.r.priority[0] + obj.r.priority[1] + obj.r.priority[2] + obj.r.priority[3] == 0) return;

  if(!obj.r.aboveEnable && !obj.r.belowEnable) return;

  for(uint s : range(34)) {
    if(obj_tileList[s].tile == 0xffff) continue;
    obj_renderTile(s);
  }

  buildWindowTables(Object::ID::OBJ);
  uint8* wt_above = windowCache[Object::ID::OBJ].above;
  uint8* wt_below = windowCache[Object::ID::OBJ].below;

  #define setpixel_above(x) \
    if(pixelCache[x].abovePriority < pri) { \
      pixelCache[x].abovePriority = pri; \
      pixelCache[x].aboveLayer = Object::ID::OBJ; \
      pixelCache[x].aboveColor = getPalette(obj_linePalette[x]); \
      pixelCache[x].aboveColorExemption = obj_linePalette[x] < 192; \
    }
  #define setpixel_below(x) \
    if(pixelCache[x].belowPriority < pri) { \
      pixelCache[x].belowPriority = pri; \
      pixelCache[x].belowLayer = Object::ID::OBJ; \
      pixelCache[x].belowColor = getPalette(obj_linePalette[x]); \
      pixelCache[x].belowColorExemption = obj_linePalette[x] < 192; \
    }
  for(int x : range(256)) {
    if(obj_linePriority[x] == OBJ_PRI_NONE) continue;

    uint pri = obj.r.priority[obj_linePriority[x]];
    if(obj.r.aboveEnable && !wt_above[x]) { setpixel_above(x); }
    if(obj.r.belowEnable && !wt_below[x]) { setpixel_below(x); }
  }
  #undef setpixel_above
  #undef setpixel_below
}
