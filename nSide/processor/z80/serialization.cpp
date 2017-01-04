void Z80::serialize(serializer& s) {
  s.integer(r.af.word); s.integer(r.af_.word);
  s.integer(r.bc.word); s.integer(r.bc_.word);
  s.integer(r.de.word); s.integer(r.de_.word);
  s.integer(r.hl.word); s.integer(r.hl_.word);
  s.integer(r.ix.word);
  s.integer(r.iy.word);
  s.integer(r.ir.word);
  s.integer(r.sp);
  s.integer(r.pc);

  r.ei.serialize(s);
  r.halt.serialize(s);
  r.iff1.serialize(s);
  r.iff2.serialize(s);
  s.integer(r.im);
}
