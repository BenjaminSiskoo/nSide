auto CPU::serialize(serializer& s) -> void {
  R6502::serialize(s);
  Thread::serialize(s);

  s.integer(io.rdyLine);
  s.integer(io.rdyAddrValid);
  s.integer(io.rdyAddrValue);
}
