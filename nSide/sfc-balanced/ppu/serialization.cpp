auto PPUcounter::serialize(serializer& s) -> void {
  s.integer(status.interlace);
  s.integer(status.field);
  s.integer(status.vcounter);
  s.integer(status.hcounter);

  s.array(history.field);
  s.array(history.vcounter);
  s.array(history.hcounter);
  s.integer(history.index);
}

auto PPU::serialize(serializer& s) -> void {
  Thread::serialize(s);
  PPUcounter::serialize(s);

  s.array(vram);
  s.array(oam);
  s.array(cgram);

  s.integer(ppu1_version);
  s.integer(ppu2_version);

  s.integer(line);

  s.integer(display.interlace);
  s.integer(display.overscan);

  s.integer(cache.oam_basesize);
  s.integer(cache.oam_nameselect);
  s.integer(cache.oam_tdaddr);

  s.integer(regs.ppu1_mdr);
  s.integer(regs.ppu2_mdr);
  for(uint n : range(4)) s.integer(regs.bg_y[n]);

  s.integer(regs.oam_iaddr);
  s.integer(regs.cgram_iaddr);

  s.integer(regs.display_disable);
  s.integer(regs.display_brightness);

  s.integer(regs.oam_baseaddr);
  s.integer(regs.oam_addr);
  s.integer(regs.oam_priority);

  s.integer(regs.oam_latchdata);

  s.integer(regs.bg3_priority);
  s.integer(regs.bgmode);

  s.integer(regs.mosaic_size);
  s.integer(regs.mosaic_countdown);

  s.integer(regs.bgofs_latchdata);
  s.integer(regs.mode7_hoffset);
  s.integer(regs.mode7_voffset);

  s.integer(regs.vram_incmode);
  s.integer(regs.vram_mapping);
  s.integer(regs.vram_incsize);

  s.integer(regs.vram_addr);

  s.integer(regs.mode7_repeat);
  s.integer(regs.mode7_vflip);
  s.integer(regs.mode7_hflip);

  s.integer(regs.mode7_latchdata);
  s.integer(regs.m7a);
  s.integer(regs.m7b);
  s.integer(regs.m7c);
  s.integer(regs.m7d);
  s.integer(regs.m7x);
  s.integer(regs.m7y);

  s.integer(regs.cgram_addr);

  s.integer(regs.cgram_latchdata);

  s.integer(regs.color_rgb);

  s.integer(regs.mode7_extbg);
  s.integer(regs.pseudo_hires);
  s.integer(regs.overscan);
  s.integer(regs.interlace);

  s.integer(regs.hcounter);
  s.integer(regs.vcounter);
  s.integer(regs.latch_hcounter);
  s.integer(regs.latch_vcounter);
  s.integer(regs.counters_latched);

  s.integer(regs.vram_readbuffer);

  for(uint n : range(256)) {
    s.integer(pixelCache[n].src_main);
    s.integer(pixelCache[n].src_sub);
    s.integer(pixelCache[n].bg_main);
    s.integer(pixelCache[n].bg_sub);
    s.integer(pixelCache[n].ce_main);
    s.integer(pixelCache[n].ce_sub);
    s.integer(pixelCache[n].pri_main);
    s.integer(pixelCache[n].pri_sub);
  }

  //better to just take a small speed hit than store all of bg_tiledata[3][] ...
  flush_tiledata_cache();

  for(uint n : range(6)) {
    s.array(windowCache[n].main, 256);
    s.array(windowCache[n].sub,  256);
  }

  for(uint n : range(4)) {
    s.integer(bg_info[n].tw);
    s.integer(bg_info[n].th);
    s.integer(bg_info[n].mx);
    s.integer(bg_info[n].my);
    s.integer(bg_info[n].scx);
    s.integer(bg_info[n].scy);
  }

  for(uint n : range(128)) {
    s.integer(sprite_list[n].width);
    s.integer(sprite_list[n].height);
    s.integer(sprite_list[n].x);
    s.integer(sprite_list[n].y);
    s.integer(sprite_list[n].character);
    s.integer(sprite_list[n].use_nameselect);
    s.integer(sprite_list[n].vflip);
    s.integer(sprite_list[n].hflip);
    s.integer(sprite_list[n].palette);
    s.integer(sprite_list[n].priority);
    s.integer(sprite_list[n].size);
  }
  s.integer(sprite_list_valid);
  s.integer(active_sprite);

  s.array(oam_itemlist, 32);

  for(uint n : range(34)) {
    s.integer(oam_tilelist[n].x);
    s.integer(oam_tilelist[n].y);
    s.integer(oam_tilelist[n].pri);
    s.integer(oam_tilelist[n].pal);
    s.integer(oam_tilelist[n].tile);
    s.integer(oam_tilelist[n].hflip);
  }

  s.array(oam_line_pal, 256);
  s.array(oam_line_pri, 256);

  bg1.serialize(s);
  bg2.serialize(s);
  bg3.serialize(s);
  bg4.serialize(s);
  sprite.serialize(s);
  window.serialize(s);
  screen.serialize(s);
}

auto PPU::Background::serialize(serializer& s) -> void {
  s.integer(regs.tiledata_addr);
  s.integer(regs.screen_addr);
  s.integer(regs.screen_size);
  s.integer(regs.mosaic_enabled);
  s.integer(regs.tile_size);

  s.integer(regs.main_enable);
  s.integer(regs.sub_enable);

  s.integer(regs.hoffset);
  s.integer(regs.voffset);
}

auto PPU::Sprite::serialize(serializer& s) -> void {
  s.integer(t.item_count);
  s.integer(t.tile_count);

  s.integer(regs.main_enable);
  s.integer(regs.sub_enable);
  s.integer(regs.interlace);

  s.integer(regs.base_size);
  s.integer(regs.nameselect);
  s.integer(regs.tiledata_addr);
  s.integer(regs.first_sprite);

  s.integer(regs.time_over);
  s.integer(regs.range_over);
}

auto PPU::Window::serialize(serializer& s) -> void {
  s.integer(regs.bg1_one_enable);
  s.integer(regs.bg1_one_invert);
  s.integer(regs.bg1_two_enable);
  s.integer(regs.bg1_two_invert);
  
  s.integer(regs.bg2_one_enable);
  s.integer(regs.bg2_one_invert);
  s.integer(regs.bg2_two_enable);
  s.integer(regs.bg2_two_invert);

  s.integer(regs.bg3_one_enable);
  s.integer(regs.bg3_one_invert);
  s.integer(regs.bg3_two_enable);
  s.integer(regs.bg3_two_invert);

  s.integer(regs.bg4_one_enable);
  s.integer(regs.bg4_one_invert);
  s.integer(regs.bg4_two_enable);
  s.integer(regs.bg4_two_invert);

  s.integer(regs.oam_one_enable);
  s.integer(regs.oam_one_invert);
  s.integer(regs.oam_two_enable);
  s.integer(regs.oam_two_invert);

  s.integer(regs.col_one_enable);
  s.integer(regs.col_one_invert);
  s.integer(regs.col_two_enable);
  s.integer(regs.col_two_invert);

  s.integer(regs.one_left);
  s.integer(regs.one_right);
  s.integer(regs.two_left);
  s.integer(regs.two_right);

  s.integer(regs.bg1_mask);
  s.integer(regs.bg2_mask);
  s.integer(regs.bg3_mask);
  s.integer(regs.bg4_mask);
  s.integer(regs.oam_mask);
  s.integer(regs.col_mask);

  s.integer(regs.bg1_main_enable);
  s.integer(regs.bg1_sub_enable);
  s.integer(regs.bg2_main_enable);
  s.integer(regs.bg2_sub_enable);
  s.integer(regs.bg3_main_enable);
  s.integer(regs.bg3_sub_enable);
  s.integer(regs.bg4_main_enable);
  s.integer(regs.bg4_sub_enable);
  s.integer(regs.oam_main_enable);
  s.integer(regs.oam_sub_enable);

  s.integer(regs.col_main_mask);
  s.integer(regs.col_sub_mask);
}

auto PPU::Screen::serialize(serializer& s) -> void {
  s.integer(regs.addsub_mode);
  s.integer(regs.direct_color);

  s.integer(regs.color_mode);
  s.integer(regs.color_halve);
  s.integer(regs.bg1_color_enable);
  s.integer(regs.bg2_color_enable);
  s.integer(regs.bg3_color_enable);
  s.integer(regs.bg4_color_enable);
  s.integer(regs.oam_color_enable);
  s.integer(regs.back_color_enable);

  s.integer(regs.color_b);
  s.integer(regs.color_g);
  s.integer(regs.color_r);
}
