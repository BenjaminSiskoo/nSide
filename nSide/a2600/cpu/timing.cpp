auto CPU::step(uint clocks) -> void {
  Thread::step(clocks);
  synchronize(pia);
  synchronize(tia);
}

auto CPU::lastCycle() -> void {
}

auto CPU::rdyLine(bool line) -> void {
  io.rdyLine = line;
}

auto CPU::rdyAddr(bool valid, uint16 value) -> void {
  io.rdyAddrValid = valid;
  io.rdyAddrValue = value & 0x1fff;
}
