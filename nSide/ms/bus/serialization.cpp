auto Bus::serialize(serializer& s) -> void {
  Processor::Z80::Bus::serialize(s);

  s.array(ram, ramMask + 1);

  s.integer(disableIO);
  s.integer(disableBIOS);
  s.integer(disableRAM);
  s.integer(disableMyCard);
  s.integer(disableCartridge);
  s.integer(disableExpansion);
}
