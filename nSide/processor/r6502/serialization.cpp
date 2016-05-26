auto R6502::serialize(serializer& s) -> void {
  s.integer(r.mdr);
  s.integer(r.pc);
  s.integer(r.a);
  s.integer(r.x);
  s.integer(r.y);
  s.integer(r.s);
  s.integer(r.p.n);
  s.integer(r.p.v);
  s.integer(r.p.d);
  s.integer(r.p.i);
  s.integer(r.p.z);
  s.integer(r.p.c);

  s.integer(abs.w);
  s.integer(iabs.w);
  s.integer(rd);
  s.integer(zp);
  s.integer(aa);
}
