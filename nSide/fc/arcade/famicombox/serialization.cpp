auto FamicomBox::serialize(serializer& s) -> void {
  s.array(bios_ram);
  s.array(test_ram);

  s.integer(dip);
}
