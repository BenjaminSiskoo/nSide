//MLT-ACTION52

struct MLT_Action52 : Board {
  MLT_Action52(Markup::Node& board_node) : Board(board_node) {
    settings.connected_chips = board_node["connected-chips"].natural();
    uint2 chip_block = 0;
    for(uint chip_id = 0; chip_id < 4; chip_id++) {
      if(settings.connected_chips & (0x1 << chip_id)) settings.chip_map[chip_id] = chip_block++;
    }
  }

  auto prg_read(uint addr) -> uint8 {
    if(addr & 0x8000 && settings.connected_chips & (1 << prg_chip)) {
      uint target_bank = prg_bank;
      target_bank |= settings.chip_map[prg_chip] << 5;
      if(prg_mode) return read(prgrom, (target_bank << 14) | (addr & 0x3fff));
      else         return read(prgrom, (target_bank << 14) | (addr & 0x7fff));
    }
    return cpu.mdr();
  }

  auto prg_write(uint addr, uint8 data) -> void {
    if(addr & 0x8000) {
      mirror = (addr & 0x2000) >> 13;
      prg_chip = (addr & 0x1800) >> 11;
      prg_bank = (addr & 0x07c0) >> 6;
      prg_mode = (addr & 0x0020) >> 5;
      chr_bank = ((addr & 0x000f) << 2) + (data & 0x03);
    }
  }

  auto chr_read(uint addr) -> uint8 {
    if(addr & 0x2000) {
      if(mirror) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_read(addr);
    }
    addr = (chr_bank * 0x2000) + (addr & 0x1fff);
    return Board::chr_read(addr);
  }

  auto chr_write(uint addr, uint8 data) -> void {
    if(addr & 0x2000) {
      if(mirror) addr = ((addr & 0x0800) >> 1) | (addr & 0x03ff);
      return ppu.ciram_write(addr, data);
    }
    addr = (chr_bank * 0x2000) + (addr & 0x1fff);
    Board::chr_write(addr, data);
  }

  auto power() -> void {
  }

  auto reset() -> void {
    mirror = 0;
    prg_chip = 0;
    prg_bank = 0;
    prg_mode = 0;
    chr_bank = 0;
  }

  auto serialize(serializer &s) -> void {
    Board::serialize(s);
    s.integer(mirror);
    s.integer(prg_chip);
    s.integer(prg_bank);
    s.integer(prg_mode);
    s.integer(chr_bank);
  }

  struct Settings {
    uint4 connected_chips;
    uint2 chip_map[4];
  } settings;

  bool mirror;
  uint2 prg_chip;
  uint5 prg_bank;
  bool prg_mode;
  uint6 chr_bank;
};
