auto CPU::serialize(serializer& s) -> void {
  R6502::serialize(s);
  Thread::serialize(s);

  s.array(ram);

  s.integer(status.interruptPending);
  s.integer(status.nmiPending);
  s.integer(status.nmiLine);
  s.integer(status.irqLine);
  s.integer(status.irqAPULine);

  s.integer(status.rdyLine);
  s.integer(status.rdyAddressValid);
  s.integer(status.rdyAddressValue);

  s.integer(status.oamDMAPending);
  s.integer(status.oamDMAPage);
}
