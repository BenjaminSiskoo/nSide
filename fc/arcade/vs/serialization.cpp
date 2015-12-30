auto VSArcadeBoard::serialize(serializer& s) -> void {
  s.array(ram);

  s.integer(swap_controllers);
  s.integer(dip);
}
