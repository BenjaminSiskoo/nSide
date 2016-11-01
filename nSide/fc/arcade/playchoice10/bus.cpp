auto PlayChoice10::Bus::read(uint16 addr) -> uint8 {
  if(addr < 0x8000) return bios[addr & 0x3fff];
  if(addr < 0x8800) return wram[addr & 0x07ff];
  if(addr < 0x8c00) return sram[addr & 0x03ff];
  if(addr < 0x9000) return sram[(addr & 0x03ff) | (sramBank << 10)];
  if(addr < 0x9800) return 0x00;  //VRAM is write-only
  if(addr < 0xc000) return 0x00;  //open bus
  if(addr < 0xe000) return cartridge.board->instrom.read(addr & 0x1fff);

  uint8 data = 0xe7;
  if(!promTest || promAddress < 0x40) {
    data |= (((cartridge.board->keyrom.read(promAddress >> 3) >> (promAddress & 7)) & 1) << 3) ^ 0x08;
  } else {
    data |= (promAddress & 2 ? (uint8)0x00 : cartridge.board->keyrom.read(8)) ^ 0x08;
  }
  data |= ((promAddress & 0x20) >> 1) ^ 0x10;
  return data;
}

auto PlayChoice10::Bus::write(uint16 addr, uint8 data) -> void {
  if(addr < 0x8000) return;
  if(addr < 0x8800) { wram[addr & 0x07ff] = data; return; }
  if(addr < 0x8c00) { sram[addr & 0x03ff] = data; return; }
  if(addr < 0x9000) { sram[(addr & 0x03ff) | (sramBank << 10)] = data; return; }
  if(addr < 0x9800) { playchoice10.videoCircuit.vram[addr & 0x07ff] = data; return; }
  if(addr < 0xc000) return;
  if(addr < 0xe000) return;

  promTest = data & 0x10;
  if(promClock && (data & 0x08) == 0) promAddress++;
  promClock = data & 0x08;
  if((data & 0x01) == 0) promAddress = 0;
  return;
}

auto PlayChoice10::Bus::in(uint8 addr) -> uint8 {
  uint8 data = 0x00;
  bool channelSelect = false;
  bool enter         = false;
  bool reset         = false;
  bool coin2         = false;
  bool service       = false;
  bool coin1         = false;
  switch(addr & 0x03) {
  case 0x00:
    data |= channelSelect  << 0;
    data |= enter          << 1;
    data |= reset          << 2;
    data |= cpu.io.nmiLine << 3;
    data |= 0              << 4;
    data |= coin2          << 5;
    data |= service        << 6;
    data |= coin1          << 7;
    break;
  case 0x01: data = (playchoice10.dip >> 0) & 0xff; break;
  case 0x02: data = (playchoice10.dip >> 8) & 0xff; break;
  case 0x03: data = 0x00; break;
  }
  return data;
}

auto PlayChoice10::Bus::out(uint8 addr, uint8 data) -> void {
  data &= 0x01;
  switch(addr & 0x1f) {
  case 0x00: playchoice10.vramAccess = data; break;
  case 0x01: playchoice10.controls   = data; break;
  case 0x02: playchoice10.ppuOutput  = data; break;
  case 0x03: playchoice10.apuOutput  = data; break;
  case 0x04: playchoice10.cpuReset   = data; break;
  case 0x05: playchoice10.cpuStop    = data; break;
  case 0x06: playchoice10.display    = data; break;
  case 0x08: playchoice10.z80NMI     = data; break;
  case 0x09: playchoice10.watchdog   = data; break;
  case 0x0a: playchoice10.ppuReset   = data; break;
  case 0x0b: channel.bit(0)          = data; break;
  case 0x0c: channel.bit(1)          = data; break;
  case 0x0d: channel.bit(2)          = data; break;
  case 0x0e: channel.bit(3)          = data; break;
  case 0x0f: sramBank                = data; break;
  }
  switch(addr & 0x13) {
  case 0x10: break;
  case 0x11: break;
  case 0x12: break;
  case 0x13: break;
  }
}

auto PlayChoice10::Bus::power() -> void {
  channel    = 0;  //channel 1 on-screen
  sramBank   = 1;
}

auto PlayChoice10::Bus::reset() -> void {
}
