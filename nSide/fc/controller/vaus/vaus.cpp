Vaus::Vaus(uint port) : Controller(port, Device::Vaus) {
  create(Controller::Enter, system.cpuFrequency());
  latched = 0;
  counter = 0;

  x = 0;
  control = 0;

  prev = 0;
}

auto Vaus::main() -> void {
  uint next = ppu.vcounter() * 341 + ppu.hcounter();

  if(next < prev) {
    //Vcounter wrapped back to zero; update control knob for start of new frame
    int nx = poll(Control) * 160 / 256;
    const uint8_t trimpot = 0x0d;
    x = max(trimpot, min(trimpot + 0xa0, x - nx));
  }

  prev = next;
  step(3);
}

auto Vaus::data() -> uint5 {
  return data1() << 2 | data2() << 3;
}

auto Vaus::data1() -> bool {
  return poll(Fire) << 1;
}

auto Vaus::data2() -> uint5 {
  if(latched == 1) return ((control >> 7) & 1) << 1;

  if(counter >= 8) return 0;

  bool controlData;
  switch(counter++) { default:
  case 0: controlData = ((control >> 7) & 1); break;
  case 1: controlData = ((control >> 6) & 1); break;
  case 2: controlData = ((control >> 5) & 1); break;
  case 3: controlData = ((control >> 4) & 1); break;
  case 4: controlData = ((control >> 3) & 1); break;
  case 5: controlData = ((control >> 2) & 1); break;
  case 6: controlData = ((control >> 1) & 1); break;
  case 7: controlData = ((control >> 0) & 1); break;
  }
  return controlData << 1;
}

auto Vaus::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  control = x & 0xff;
}
