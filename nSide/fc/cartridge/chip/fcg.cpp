struct FCG : Chip {
  FCG(Board& board, Markup::Node& board_node) : Chip(board) {
    string type = board_node["chip[0]/type"].text();
    if(type == "FCG-1"  ) revision = Revision::FCG1;
    if(type == "FCG-2"  ) revision = Revision::FCG2;
    if(type == "LZ93D50") revision = Revision::LZ93D50;

    eeprom = EEPROM::None;
    type = board_node["chip[1]/type"].text();
    if(type == "24C01") eeprom = EEPROM::_24C01;
    if(type == "24C02") eeprom = EEPROM::_24C02;
  }

  auto main() -> void {
    if(irq_counter_enable) {
      if(--irq_counter == 0xffff) {
        cpu.set_irq_line(1);
        irq_counter_enable = false;
      }
    }

    tick();
  }

  auto prg_addr(uint addr) const -> uint {
    bool region = addr & 0x4000;
    return ((region == 0 ? prg_bank : (uint8)0x0f) << 14) | (addr & 0x3fff);
  }

  auto chr_addr(uint addr) const -> uint {
    return (chr_bank[addr >> 10] << 10) | (addr & 0x03ff);
  }

  auto ciram_addr(uint addr) const -> uint {
    switch(mirror) {
    case 0: return ((addr & 0x0400) >> 0) | (addr & 0x03ff);
    case 1: return ((addr & 0x0800) >> 1) | (addr & 0x03ff);
    case 2: return 0x0000 | (addr & 0x03ff);
    case 3: return 0x0400 | (addr & 0x03ff);
    }
  }

  auto ram_read(uint addr) -> uint8 {
    return board.read(board.prgram, addr & 0x1fff);
  }

  auto ram_write(uint addr, uint8 data) -> void {
    board.write(board.prgram, addr & 0x1fff, data);
  }

  auto reg_write(uint addr, uint8 data) -> void {
    switch(addr & 0x0f) {
    case 0x00: case 0x01: case 0x02: case 0x03:
    case 0x04: case 0x05: case 0x06: case 0x07:
      chr_bank[addr & 7] = data;
      break;
    case 0x08:
      prg_bank = data & 0x0f;
      break;
    case 0x09:
      mirror = data & 0x03;
      break;
    case 0x0a:
      cpu.set_irq_line(0);
      irq_counter_enable = data & 0x01;
      irq_counter = irq_latch;
      break;
    case 0x0b:
      irq_latch = (irq_latch & 0xff00) | (data << 0);
      break;
    case 0x0c:
      irq_latch = (irq_latch & 0x00ff) | (data << 8);
      break;
    case 0x0d:
      //TODO: serial EEPROM support
      eeprom_i2c_scl = data & 0x20;
      eeprom_enable_read = data & 0x80;
      break;
    }
  }

  auto power() -> void {
    reset();
  }

  auto reset() -> void {
    for(auto &n : chr_bank) n = 0;
    prg_bank = 0;
    mirror = 0;
    irq_counter_enable = 0;
    irq_counter = 0;
    irq_latch = 0;
    eeprom_i2c_scl = 0;
    eeprom_enable_read = 0;
  }

  auto serialize(serializer& s) -> void {
    s.array(chr_bank);
    s.integer(prg_bank);
    s.integer(mirror);
    s.integer(irq_counter_enable);
    s.integer(irq_counter);
    s.integer(irq_latch);
    s.integer(eeprom_i2c_scl);
    s.integer(eeprom_enable_read);
  }

  enum class Revision : uint {
    FCG1,
    FCG2,
    LZ93D50,
  } revision;

  enum class EEPROM : uint {
    None,
    _24C01,
    _24C02
  } eeprom;

  uint8 chr_bank[8];
  uint8 prg_bank;
  uint2 mirror;
  bool irq_counter_enable;
  uint16 irq_counter;
  uint16 irq_latch;
  bool eeprom_i2c_scl; // When using SRAM, enables RAM chip
  bool eeprom_enable_read;
};
