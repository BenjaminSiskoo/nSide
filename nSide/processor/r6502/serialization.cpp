auto R6502::serialize(serializer& s) -> void {
  s.integer(r.mdr);
  s.integer(r.pc.w);
  s.integer(r.a);
  s.integer(r.x);
  s.integer(r.y);
  s.integer(r.s);
  s.integer(r.p.b);

  s.integer(abs.w);
  s.integer(iabs.w);
  s.integer(rd);
  s.integer(zp);
  s.integer(aa);
}
