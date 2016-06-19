Vaus::Vaus(bool port, uint index) : Controller(port, index) {
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
    int nx = interface->inputPoll(port, index, Control) * 160 / 256;
    const uint8_t trimpot = 0x0d;
    x = max(trimpot, min(trimpot + 0xa0, x - nx));
  }

  prev = next;
  step(3);
}

auto Vaus::data() -> uint3 {
  bool fire = interface->inputPoll(port, index, Fire);
  if(latched == 1) return fire << 1 | control.bit(7) << 2;
  if(counter >= 8) return 0;

  bool controlData;
  switch(counter++) { default:
  case 0: controlData = control.bit(7); break;
  case 1: controlData = control.bit(6); break;
  case 2: controlData = control.bit(5); break;
  case 3: controlData = control.bit(4); break;
  case 4: controlData = control.bit(3); break;
  case 5: controlData = control.bit(2); break;
  case 6: controlData = control.bit(1); break;
  case 7: controlData = control.bit(0); break;
  }
  return fire << 1 | controlData << 2;
}

auto Vaus::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  control = x & 0xff;
}
