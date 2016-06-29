auto PPU::Object::update(uint addr, uint8 data) -> void {
  if(addr < 0x0200) {
    uint n = addr >> 2;
    switch(addr & 3) {
    case 0: list[n].x = (list[n].x & 0x0100) | data; break;
    case 1: list[n].y = (data + 1) & 0xff; break;
    case 2: list[n].character = data; break;
    case 3: list[n].vflip = data & 0x80;
            list[n].hflip = data & 0x40;
            list[n].priority = (data >> 4) & 3;
            list[n].palette = (data >> 1) & 7;
            list[n].nameSelect = data & 0x01;
    }
  } else {
    uint n = (addr & 0x1f) << 2;  //sprite#
    list[n + 0].x        = ((data & 0x01) << 8) | (list[n + 0].x & 0xff);
    list[n + 0].size     = data & 0x02;
    list[n + 1].x        = ((data & 0x04) << 6) | (list[n + 1].x & 0xff);
    list[n + 1].size     = data & 0x08;
    list[n + 2].x        = ((data & 0x10) << 4) | (list[n + 2].x & 0xff);
    list[n + 2].size     = data & 0x20;
    list[n + 3].x        = ((data & 0x40) << 2) | (list[n + 3].x & 0xff);
    list[n + 3].size     = data & 0x80;
  }
}
