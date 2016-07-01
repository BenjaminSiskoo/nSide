auto CPU::serialize(serializer& s) -> void {
  R6502::serialize(s);
  Thread::serialize(s);

  s.array(ram);

  s.integer(status.interruptPending);
  s.integer(status.nmiPending);
  s.integer(status.nmiLine);
  s.integer(status.irqLine);
  s.integer(status.apuLine);

  s.integer(status.rdyLine);
  s.integer(status.rdyAddrValid);
  s.integer(status.rdyAddrValue);

  s.integer(io.oamdmaPending);
  s.integer(io.oamdmaPage);
}
