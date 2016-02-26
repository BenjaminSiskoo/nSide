auto VSArcadeBoard::serialize(serializer& s) -> void {
  s.array(ram);
  s.integer(ramSide);

  s.integer(swapControllers);
  s.integer(mainDip);
  s.integer(subDip);
  s.integer(watchdog);
}
