auto PlayChoice10::Bus::read(uint16 addr) -> uint8 {
  if(addr < 0x8000) return bios[addr & 0x3fff];
  if(addr < 0x8800) return wram[addr & 0x07ff];
  if(addr < 0x8c00) return sram[addr & 0x03ff];
  if(addr < 0x9000) return sram[(addr & 0x03ff) | (sramBank << 10)];
  if(addr < 0x9800) return 0x00;  //VRAM is write-only
  if(addr < 0xc000) return 0x00;  //open bus
  if(channel != 0) return 0xff;
  if(addr < 0xe000) return cartridge.board->instrom.read(addr & 0x1fff);

  //PROM
  uint8 data = 0xe7;
  uint8 byte;
  if(!promTest || !promAddress.bit(6)) {
    byte = cartridge.board->keyrom.read((promAddress & 0x3f) >> 3);
  } else {
    byte = promAddress.bit(4) ? (uint8)0x00 : cartridge.board->keyrom.read(8);
  }
  data.bit(3) = !byte.bit(promAddress & 7);
  data.bit(4) = !promAddress.bit(5);
  return data;
}

auto PlayChoice10::Bus::write(uint16 addr, uint8 data) -> void {
  if(addr < 0x8000) return;
  if(addr < 0x8800) { wram[addr & 0x07ff] = data; return; }
  if(addr < 0x8c00) { sram[addr & 0x03ff] = data; return; }
  if(addr < 0x9000) { sram[(addr & 0x03ff) | (sramBank << 10)] = data; return; }
  if(addr < 0x9800) { playchoice10.videoCircuit.writeVRAM(addr, data); return; }
  if(addr < 0xc000) return;
  if(addr < 0xe000) return;

  promTest = data.bit(4);
  if(promClock && data.bit(3) == 0) promAddress++;
  promClock = data.bit(3);
  if(data.bit(0) == 0) promAddress = 0;
  return;
}

auto PlayChoice10::Bus::in(uint8 addr) -> uint8 {
  //print("IN  ", hex(addr, 4L), "\n");
  bool channelSelect = poll(ChannelSelect);
  bool enter         = poll(Enter);
  bool reset         = poll(Reset);
  bool nmi           = !playchoice10.nmiDetected;
  bool coin2         = false;
  bool service       = poll(ServiceButton);
  bool coin1         = false;
  switch(addr & 0x03) {
  case 0x00:
    return channelSelect << 0 | enter << 1 | reset << 2 | nmi << 3
    | coin2 << 5 | service << 6 | coin1 << 7;
  case 0x01: return playchoice10.dip.byte(0);
  case 0x02: return playchoice10.dip.byte(1);
  case 0x03: return playchoice10.nmiDetected = false, 0x00;
  }
  unreachable;
}

auto PlayChoice10::Bus::out(uint8 addr, uint8 data) -> void {
  //print("OUT ", hex(addr, 4L), " ", hex(data, 2L), "\n");
  data &= 0x01;
  switch(addr & 0x1f) {
  case 0x00: {
    playchoice10.vramAccess = data;
    break;
  }
  case 0x01: {
    playchoice10.controls   = data;
    print(data ? "Enable" : "Disable", " Controls\n");
    break;
  }
  case 0x02: {
    playchoice10.ppuOutput  = data;
    print(data ? "Enable" : "Disable", " PPU\n");
    break;
  }
  case 0x03: {
    playchoice10.apuOutput  = 1;//data;
    print(data ? "Enable" : "Disable", " Sound\n");
    break;
  }
  case 0x04: {
    if(!playchoice10.cpuReset && data) cpu.reset(), apu.reset();
    playchoice10.cpuReset   = data;
    print(data ? "End Resetting" : "Reset", " CPU\n");
    break;
  }
  case 0x05: {
    playchoice10.cpuStop    = data;
    print(data ? "Start" : "Stop", " CPU\n");
    break;
  }
  case 0x06: playchoice10.display    = data; break;
  case 0x08: {
    playchoice10.z80NMI     = data;
    print(data ? "Enable" : "Disable", " Z80 NMI\n");
    break;
  }
  case 0x09: {
    playchoice10.watchdog   = data;
    print(data ? "Enable" : "Disable", " Watchdog\n");
    break;
  }
  case 0x0a: {
    if(!playchoice10.ppuReset && data) ppu.reset();
    playchoice10.ppuReset   = data;
    print(data ? "End Resetting" : "Reset", " PPU\n");
    break;
  }
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

auto PlayChoice10::Bus::poll(uint input) -> int16 {
  return interface->inputPoll(ID::Port::Arcade, ID::Device::PC10Panel, (uint)input);
}

auto PlayChoice10::Bus::power() -> void {
  channel    = 0;  //channel 1 on-screen
  sramBank   = 1;
}

auto PlayChoice10::Bus::reset() -> void {
  promAddress = 0;
}
