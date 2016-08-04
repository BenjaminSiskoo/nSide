auto PIA::serialize(serializer& s) -> void {
  Thread::serialize(s);

  s.array(ram);

  s.integer(io.color);
  s.integer(io.difficulty0);
  s.integer(io.difficulty1);

  s.integer(io.swcha);
  s.integer(io.swacnt);
  s.integer(io.swchb);
  s.integer(io.swbcnt);

  s.integer(io.timer);
  s.integer(io.timerMask);

  s.integer(io.timerInterrupt);
  s.integer(io.pa7Interrupt);
  s.integer(io.edgeDetect);
}
