VausE::VausE(uint index) : Expansion(index) {
  create(Expansion::Enter, system.cpuFrequency());
  latched = 0;
  counter = 0;

  x = 0;
  control = 0;

  prev = 0;
}

auto VausE::main() -> void {
  uint next = ppu.vcounter() * 341 + ppu.hcounter();

  if(next < prev) {
    //Vcounter wrapped back to zero; update control knob for start of new frame
    int nx = interface->inputPoll(Port::Expansion, index, Control) * 160 / 256;
    const uint8_t trimpot = 0x0d;
    x = max(trimpot, min(trimpot + 0xa0, x - nx));
  }

  prev = next;
  step(3);
}

auto VausE::data1() -> bool {
  return interface->inputPoll(Port::Expansion, index, Fire);
}

auto VausE::data2() -> uint5 {
  if(latched == 1) return control.bit(7) << 1;
  if(counter >= 8) return 0;

  return control.bit(7 - counter++) << 1;
}

auto VausE::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;

  control = x & 0xff;
}
