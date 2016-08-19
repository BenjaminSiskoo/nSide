void Z80::serialize(serializer& s) {
  s.integer(r.af);
  s.integer(r.bc);
  s.integer(r.de);
  s.integer(r.hl);
  s.integer(r.ix);
  s.integer(r.iy);
  s.integer(r.sp);
  s.integer(r.pc);
  s.integer(r.i);
  s.integer(r.r);

  s.integer(r.halt);
  s.integer(r.stop);
  s.integer(r.ei);
  s.integer(r.ime);
}
